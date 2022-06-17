#pragma once

#include <stack>

#include <entt/entity/fwd.hpp>

namespace Plop
{
	/*
	
	usage:

	register what to do with
	UndoManager::RegisterActionCommands([](whatev){ return true;}, [](whatev){ return true;});

	register an action with
	UndoManager::RegisterAction(UndoAction::Move(vOldPos, vNewPos));

	undo with
	UndoManager::Undo();	
	
	*/




	class UndoManager;
	class UndoAction
	{
		friend class UndoManager;

	public:

		enum class Type
		{
			MOVE,
			ROTATE,
			SCALE,
			GIZMO_MANIPULATE,

			COUNT
		};

		struct UndoData
		{
			union
			{
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
		[[nodiscard]] static UndoAction MoveEntity(entt::entity _enttID, const glm::vec3 &_vOld, const glm::vec3 &_vNew);
		[[nodiscard]] static UndoAction RotateEntity(entt::entity _enttID, const glm::quat &_qOld, const glm::quat &_qNew);
		[[nodiscard]] static UndoAction ScaleEntity(entt::entity _enttID, const glm::vec3 &_vOld, const glm::vec3 &_vNew);
		[[nodiscard]] static UndoAction GizmoManipulateEntity(entt::entity _enttID, const glm::mat4 &_mOld, const glm::mat4& _mNew);
	};

	class UndoManager
	{
	public:

		using Callback = std::function<bool(const UndoAction::UndoData&)>;

		static void RegisterActionCommands(UndoAction::Type _eType, Callback &&_cbUndo, Callback &&_cbRedo);

		static void RegisterAction(UndoAction &&_action);
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
