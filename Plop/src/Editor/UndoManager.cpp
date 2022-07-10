#include "Plop_pch.h"
#include "UndoManager.h"

namespace Plop
{

	UndoAction UndoAction::EntityMove(entt::entity _enttID, const glm::vec3& _vOld, const glm::vec3& _vNew)
	{
		UndoAction action;
		action.m_eType = Type::ENTITY_MOVE;
		action.m_data.entityVec3.enttID = _enttID;
		action.m_data.entityVec3.vOld = _vOld;
		action.m_data.entityVec3.vNew = _vNew;
		return action;
	}

	UndoAction UndoAction::EntityRotate(entt::entity _enttID, const glm::quat& _qOld, const glm::quat& _qNew)
	{
		UndoAction action;
		action.m_eType = Type::ENTITY_ROTATE;
		action.m_data.entityQuat.enttID = _enttID;
		action.m_data.entityQuat.qOld = _qOld;
		action.m_data.entityQuat.qNew = _qNew;
		return action;
	}

	UndoAction UndoAction::EntityScale(entt::entity _enttID, const glm::vec3& _vOld, const glm::vec3& _vNew)
	{
		UndoAction action;
		action.m_eType = Type::ENTITY_SCALE;
		action.m_data.entityVec3.enttID = _enttID;
		action.m_data.entityVec3.vOld = _vOld;
		action.m_data.entityVec3.vNew = _vNew;
		return action;
	}

	UndoAction UndoAction::EntityGizmoManipulate(entt::entity _enttID, const glm::mat4& _mOld, const glm::mat4& _mNew)
	{
		UndoAction action;
		action.m_eType = Type::ENTITY_GIZMO_MANIPULATE;
		action.m_data.entityMat4.enttID = _enttID;
		action.m_data.entityMat4.mOld = _mOld;
		action.m_data.entityMat4.mNew = _mNew;
		return action;
	}

	UndoAction UndoAction::EntityVisibility(entt::entity _enttID, bool _bBefore, bool _bAfter)
	{
		UndoAction action;
		action.m_eType = Type::ENTITY_VISIBILITY;
		action.m_data.entityVisibility.enttID = _enttID;
		action.m_data.entityVisibility.bValueBefore = _bBefore;
		action.m_data.entityVisibility.bValueAfter = _bAfter;
		return action;
	}

	std::stack<UndoAction> UndoManager::m_undoStack;
	std::stack<UndoAction> UndoManager::m_redoStack;
	UndoManager::Callback UndoManager::m_undoCommands[(std::size_t)UndoAction::Type::COUNT];
	UndoManager::Callback UndoManager::m_redoCommands[(std::size_t)UndoAction::Type::COUNT];

	void UndoManager::RegisterActionCommands(UndoAction::Type _eType, Callback &&_cbUndo, Callback &&_cbRedo)
	{
		m_undoCommands[(std::size_t)_eType] = _cbUndo;
		m_redoCommands[(std::size_t)_eType] = _cbRedo;
	}

	void UndoManager::PushAction(UndoAction &&_action)
	{
		m_undoStack.push(_action);

		// and clear the redo stack since we override the history from that point
		while (!m_redoStack.empty())
			m_redoStack.pop();
	}

	void UndoManager::Undo()
	{
		if (CanUndo())
		{
			const UndoAction &action = m_undoStack.top();

			auto& func = m_undoCommands[(std::size_t)action.m_eType];
			ASSERTM(func, "The undo commands for action type {} was not defined", action.m_eType);
			if (func)
			{
				if (func(action.m_data))
				{
					m_redoStack.push(action);
				}
				else
				{
					ASSERTM(false, "The undo command failed, redo will not be possible");
				}
				m_undoStack.pop();
			}
		}
	}

	void UndoManager::Redo()
	{
		if (CanRedo())
		{
			const UndoAction& action = m_redoStack.top();

			auto& func = m_redoCommands[(std::size_t)action.m_eType];
			ASSERTM(func, "The redo commands for action type {} was not defined", action.m_eType);
			if (func)
			{
				if (func(action.m_data))
				{

					m_undoStack.push(action);
				}
				else
				{
					ASSERTM(false, "The undo command failed, redo will not be possible");
				}
				m_redoStack.pop();
			}
		}
	}

	void UndoManager::Clear()
	{
		while (!m_undoStack.empty())
			m_undoStack.pop();
		while (!m_redoStack.empty())
			m_redoStack.pop();
	}
}
