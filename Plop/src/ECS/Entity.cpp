#include "Plop_pch.h"
#include "Entity.h"

#include <functional>

#include "Application.h"
#include "ECS/ComponentManager.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/ComponentIncludes.h"
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Serialisation.h"
#include "Utils/JsonTypes.h"
#include "Editor/EditorLayer.h"

namespace Plop
{

#pragma region CTORS
	Entity::Entity(entt::null_t)
		: m_EntityId(entt::null)
	{
		m_xLevel.reset();
	}

	Entity::Entity(entt::entity _entityID, const LevelBaseWeakPtr &_xLevel)
		: m_EntityId(_entityID)
		, m_xLevel(_xLevel)
	{}

	Entity::Entity(const Entity &_other) noexcept
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;
	}

	Entity::Entity(Entity &&_other) noexcept
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;

		// make sure the other entity is no longer valid
		_other.m_EntityId = entt::null;
		_other.m_xLevel.reset();
	}

	void Entity::Reset()
	{
		m_EntityId = entt::null;
		m_xLevel.reset();
	}
#pragma endregion

#pragma region OPERATORS
	Entity::operator bool() const
	{
		if (m_EntityId == entt::null || m_xLevel.expired())
			return false;

		return m_xLevel.lock()->m_ENTTRegistry.valid(m_EntityId);
	}

	Entity &Entity::operator=(const Entity &_other)
	{
		m_EntityId = _other.m_EntityId;
		m_xLevel = _other.m_xLevel;

		return *this;
	}

	bool Entity::operator==(const Entity &_other)
	{
		return m_EntityId == _other.m_EntityId && (!m_xLevel.expired() && !_other.m_xLevel.expired() && m_xLevel.lock() == _other.m_xLevel.lock());
	}

	bool Entity::operator!=(const Entity &_other)
	{
		return !operator==(_other);
	}
#pragma endregion

#pragma region HIERARCHY
	Entity Entity::GetParent() const
	{
		Entity parent(entt::null, m_xLevel);

		if (m_EntityId != entt::null && !m_xLevel.expired())
		{
			auto &reg = m_xLevel.lock()->m_ENTTRegistry;
			if (reg.valid(m_EntityId))
			{
				auto &graphNode = reg.get<Component_GraphNode>(m_EntityId);

				parent.m_EntityId = graphNode.parent;
			}
		}

		return parent;
	}

	void Entity::SetParent(Entity &_Parent)
	{
		ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");

		auto &reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid(m_EntityId))
		{
			auto &graphNode = reg.get<Component_GraphNode>(m_EntityId);

			Entity oldParent(graphNode.parent, m_xLevel);
			if (oldParent != _Parent)
			{
				if (oldParent)
				{
					oldParent.RemoveChild(*this);
				}
				graphNode.parent = _Parent.m_EntityId;

				if (_Parent)
				{
					_Parent.AddChild(*this);
				}
			}
		}
	}

	void Entity::GetChildren(std::vector<Entity> &_outvecChildren) const
	{
		_outvecChildren.clear();

		if (m_EntityId != entt::null && !m_xLevel.expired())
		{
			auto &reg = m_xLevel.lock()->m_ENTTRegistry;
			if (reg.valid(m_EntityId))
			{
				const auto &graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

#ifdef ALLOW_DYNAMIC_CHILDREN
				auto childEntity = graphNodeParent.firstChild;
				while (childEntity != entt::null)
				{
					_outvecChildren.emplace_back(childEntity, m_xLevel);
					childEntity = reg.get<Component_GraphNode>(childEntity).nextSibling;
				}
#else
				for (int i = 0; i < graphNodeParent.nbChild; ++i)
				{
					_outvecChildren.emplace_back(graphNodeParent.children[i], m_xLevel);
				}
#endif
			}
		}
	}
	void Entity::AddChild(Entity &_Child)
	{
		ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");
		ASSERTM(_Child, "Invalid Entity for _Child");

		auto &reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid(m_EntityId))
		{
			auto &graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

#ifdef ALLOW_DYNAMIC_CHILDREN
			if (graphNodeParent.firstChild != entt::null)
			{
				auto &graphNodeChild = reg.get<Component_GraphNode>(graphNodeParent.firstChild);
				graphNodeChild.prevSibling = _Child;

				auto &graphNodeNewChild = reg.get<Component_GraphNode>(_Child);
				graphNodeNewChild.nextSibling = graphNodeParent.firstChild;
			}

			graphNodeParent.firstChild = _Child;
#else
			ASSERTM(graphNodeParent.nbChild + 1 < Component_GraphNode::MAX_CHILDREN, "No room for another child");
			if (graphNodeParent.nbChild + 1 < graphNodeParent.MAX_CHILDREN)
			{
				graphNodeParent.children[graphNodeParent.nbChild++] = _Child.m_EntityId;
			}
#endif
		}
	}

	void Entity::RemoveChild(Entity &_Child)
	{
		ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");
		ASSERTM(_Child, "Invalid Entity for _Child");

		auto &reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid(m_EntityId))
		{

			auto &graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

#ifdef ALLOW_DYNAMIC_CHILDREN

			const auto &graphNodeChild = reg.get<Component_GraphNode>(_Child);

			if (graphNodeParent.firstChild == _Child.m_EntityId)
			{
				graphNodeParent.firstChild = graphNodeChild.nextSibling;
			}
			else //if (graphNodeChild.prevSibling != entt::null)
			{
				auto &graphNodePrev = reg.get<Component_GraphNode>(graphNodeChild.prevSibling);
				graphNodePrev.nextSibling = graphNodeChild.nextSibling;
			}
			if (graphNodeChild.nextSibling != entt::null)
			{
				auto &graphNodeNext = reg.get<Component_GraphNode>(graphNodeChild.nextSibling);
				graphNodeNext.prevSibling = graphNodeChild.prevSibling;
			}

#else
			for (int i = 0; i < graphNodeParent.nbChild; ++i)
			{
				if (graphNodeParent.children[i] == _Child.m_EntityId)
				{
					graphNodeParent.children[i] = std::move(graphNodeParent.children[graphNodeParent.nbChild - 1]);
					graphNodeParent.nbChild--;
					break;
				}
			}
#endif
		}
	}
#pragma endregion

#pragma region COMPONENTS

	std::map<String, std::function<void(Entity)>>	Entity::s_mapAddComponent;

	void Entity::EditorUI()
	{
		s_mapAddComponent.clear();

		auto &registry = m_xLevel.lock()->m_ENTTRegistry;

		if (ImGui::CollapsingHeader("Node"))
		{
			bool bFlag = HasFlag(EntityFlag::DYNAMIC_GENERATION);
			if (ImGui::Checkbox("DYNAMIC_GENERATION", &bFlag))
				SetFlag(EntityFlag::DYNAMIC_GENERATION, bFlag);
		}

		ComponentManager::EditorUI(registry, m_EntityId);



	}
#pragma endregion

#pragma region FLAGS
	void Entity::AddFlag(EntityFlag _flag)
	{
		GetComponent<Component_GraphNode>().uFlags |= _flag;
	}

	void Entity::RemoveFlag(EntityFlag _flag)
	{
		EntityFlagBits bit = _flag;
		GetComponent<Component_GraphNode>().uFlags &= ~bit;
	}

	void Entity::SetFlag(EntityFlag _flag, bool _bSet)
	{
		if (_bSet)
			AddFlag(_flag);
		else
			RemoveFlag(_flag);
	}

	bool Entity::HasFlag(EntityFlag _flag) const
	{
		return GetComponent<Component_GraphNode>().uFlags.Has(_flag);
	}
#pragma endregion

#pragma region SERIALIZATION
	void to_json(json &j, const Entity &e)
	{
		j = e.ToJson();
	}

	json Entity::ToJson() const
	{
		json j; // = EditorLayer::GetJsonEntity( *this );


		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		auto &registry = xLevel->m_ENTTRegistry;

		j["HintID"] = m_EntityId;
		j["Name"] = GetComponent<Component_Name>().sName;
		j[JSON_GUID] = GetComponent<Component_Name>().guid;
		ChildVisitor([&j](Entity &_child) {
			if (!_child.HasFlag(EntityFlag::DYNAMIC_GENERATION))
			{
				GUID guid = _child.GetComponent<Component_Name>().guid;
				j[JSON_CHILDREN].push_back(guid);
			}
		});

		ComponentManager::ToJson(registry, m_EntityId, j);

		return j;
	}

	void Entity::FromJson(const json &_jEntity)
	{
		GetComponent<Component_Name>().sName = _jEntity["Name"];

		LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
		if (_jEntity.contains(JSON_CHILDREN))
		{
			ASSERTM(xLevel != nullptr, "No current level");
			for (auto &id : _jEntity[JSON_CHILDREN])
			{
				Entity e = xLevel->GetEntityFromGUID(id);
				e.SetParent(*this);
			}
		}

		if (_jEntity.contains("Components"))
		{
			auto &registry = xLevel->m_ENTTRegistry;
			ComponentManager::FromJson(registry, m_EntityId, _jEntity["Components"]);
		}
	}
#pragma endregion

}
