#pragma once

#include "Debug/Debug.h"

#include <entt/entity/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <stack>

namespace Plop
{
	/*
	
		Usage:

		- register what to do with
		UndoManager::RegisterActionCommands([](const UndoAction::UndoData&)->bool{ foo; return true;}, [](const UndoAction::UndoData&)->bool{ bar; return true;});

		- register an action with
		UndoManager::RegisterAction(UndoAction::Whatever(old, new));

		- undo with
		UndoManager::Undo();	


		TODO:
		- use a circle buffer to avoid having to many operations
		- add Undo group to register multiple actions into one operation
	
	*/




	class UndoManager;
	class UndoAction
	{
		friend class UndoManager;

	public:

		enum class Type
		{
			ENTITY_MOVE,
			ENTITY_ROTATE,
			ENTITY_SCALE,
			ENTITY_GIZMO_MANIPULATE,
			ENTITY_VISIBILITY,

			COUNT
		};

		struct UndoData
		{
			union
			{
				struct
				{
					entt::entity enttID;
					bool bValueBefore;
					bool bValueAfter;
				} entityVisibility;
				struct
				{
					entt::entity enttID;
					glm::vec3 vOld;
					glm::vec3 vNew;
				} entityVec3;
				struct
				{
					entt::entity enttID;
					glm::quat qOld;
					glm::quat qNew;
				} entityQuat;
				struct
				{
					entt::entity enttID;
					glm::mat4 mOld;
					glm::mat4 mNew;
				} entityMat4;
			};
		};

	private:
		UndoAction() = default;

		UndoData m_data;
		Type m_eType;

	public:
		[[nodiscard]] static UndoAction EntityMove(entt::entity _enttID, const glm::vec3 &_vOld, const glm::vec3 &_vNew);
		[[nodiscard]] static UndoAction EntityRotate(entt::entity _enttID, const glm::quat &_qOld, const glm::quat &_qNew);
		[[nodiscard]] static UndoAction EntityScale(entt::entity _enttID, const glm::vec3 &_vOld, const glm::vec3 &_vNew);
		[[nodiscard]] static UndoAction EntityGizmoManipulate(entt::entity _enttID, const glm::mat4& _mOld, const glm::mat4& _mNew);
		[[nodiscard]] static UndoAction EntityVisibility(entt::entity _enttID, bool _bBefore, bool _bAfter);
	};

	class UndoManager
	{
	public:

		/// <summary>
		/// return true if the undo/redo was correctly applied
		/// return false otherwise (allows to see discrepancies and to avoid adding back to the redo/undo stack)
		/// </summary>
		using Callback = std::function<bool(const UndoAction::UndoData&)>;

		static void RegisterActionCommands(UndoAction::Type _eType, Callback &&_cbUndo, Callback &&_cbRedo);

		static void PushAction(UndoAction &&_action);
		static void Undo();
		static void Redo();
		static void Clear();

		static bool CanUndo() { return !m_undoStack.empty(); }
		static bool CanRedo() { return !m_redoStack.empty(); }

	private:
		static std::stack<UndoAction> m_undoStack;
		static std::stack<UndoAction> m_redoStack;

		static Callback m_undoCommands[(std::size_t)UndoAction::Type::COUNT];
		static Callback m_redoCommands[(std::size_t)UndoAction::Type::COUNT];
	};
}
