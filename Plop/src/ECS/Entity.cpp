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

#ifdef USE_ENTITY_HANDLE
#pragma region CTORS

	Entity::Entity(entt::entity _entityID, entt::registry& _reg)
		: m_hEntity(_reg, _entityID)
	{

	}

	Entity::Entity(entt::handle _hEntity)
		: m_hEntity(_hEntity)
	{

	}

	Entity::Entity(const Entity& _other) noexcept
		: m_hEntity(_other.m_hEntity)
	{
	}

	Entity::Entity(Entity&& _other) noexcept
		: m_hEntity(_other.m_hEntity)
	{
		// make sure the other entity is no longer valid
		_other.m_hEntity = static_cast<entt::entity>(entt::null);
	}

	void Entity::Reset()
	{
		 m_hEntity = static_cast<entt::entity>(entt::null);
	}

	bool Entity::IsFromLevel(GUID _guidLevel) const
	{
		if (m_hEntity)
		{
			GUID *pGUID = m_hEntity.registry().try_ctx<GUID>();
			ASSERTM(pGUID, "No GUID for the level of this Entity");
			if (pGUID && *pGUID == _guidLevel)
				return true;
		}
		return false;
	}

#pragma endregion

#pragma region OPERATORS
	Entity::operator bool() const
	{
		return (bool)m_hEntity;
	}

	Entity &Entity::operator=(const Entity &_other)
	{
		m_hEntity = _other.m_hEntity;

		return *this;
	}

	bool Entity::operator==(const Entity &_other)
	{
		if (!m_hEntity && !_other.m_hEntity)
			return true;
		else if (!_other.m_hEntity)
			return false;
		else if (!m_hEntity)
			return false;

		GUID* guidThis = m_hEntity.registry().try_ctx<Plop::GUID>();
		GUID* guidOther = _other.m_hEntity.registry().try_ctx<Plop::GUID>();
		if (guidThis && guidOther)
		{
			if (*guidThis != *guidOther)
				return false;
		}

		return m_hEntity == _other.m_hEntity;
	}

	bool Entity::operator!=(const Entity &_other)
	{
		return !(*this==_other);
	}
#pragma endregion

#pragma region HIERARCHY
	Entity Entity::GetParent() const
	{
		Entity parent(entt::null, m_hEntity.registry());

		if (m_hEntity)
		{
			auto &graphNode = m_hEntity.get<Component_GraphNode>();

			parent.m_hEntity = graphNode.parent;
		}

		return parent;
	}

	void Entity::SetParent(Entity &_Parent)
	{
		ASSERTM(m_hEntity, "Invalid Entity");

		if (m_hEntity)
		{
			auto &graphNode = m_hEntity.get<Component_GraphNode>();

			Entity oldParent(graphNode.parent, m_hEntity.registry());
			if (oldParent != _Parent)
			{
				if (oldParent)
				{
					oldParent.RemoveChild(*this);
				}
				graphNode.parent = _Parent.m_hEntity.entity();

				if (_Parent)
				{
					_Parent.AddChild(*this);
				}
			}
		}
	}

	bool Entity::HasChildren() const
	{
		if (m_hEntity)
		{
			auto &graphNodeParent = m_hEntity.get<Component_GraphNode>();

#ifdef ALLOW_DYNAMIC_CHILDREN
			return graphNodeParent.firstChild != entt::null;
#else
			return graphNodeParent.nbChild != 0;
#endif
		}

		return false;
	}

	void Entity::GetChildren(std::vector<Entity> &_outvecChildren) const
	{
		_outvecChildren.clear();

		if (m_hEntity)
		{
			auto& graphNodeParent = m_hEntity.get<Component_GraphNode>();

#ifdef ALLOW_DYNAMIC_CHILDREN
			auto childEntity = graphNodeParent.firstChild;
			while (childEntity != entt::null)
			{
				_outvecChildren.emplace_back(childEntity, m_hEntity.registry());
				childEntity = m_hEntity.registry().get<Component_GraphNode>(childEntity).nextSibling;
			}
#else
			for (int i = 0; i < graphNodeParent.nbChild; ++i)
			{
				_outvecChildren.emplace_back(graphNodeParent.children[i], m_hEntity.registry());
			}
#endif
		}
	}

	size_t Entity::GetChildrenCount() const
	{
		if (m_hEntity)
		{
			auto &graphNodeParent = m_hEntity.get<Component_GraphNode>();

#ifdef ALLOW_DYNAMIC_CHILDREN
			size_t count = 0;
			auto childEntity = graphNodeParent.firstChild;
			while (childEntity != entt::null)
			{
				childEntity = m_hEntity.registry().get<Component_GraphNode>(childEntity).nextSibling;
				++count;
			}
			return count;
#else
			return graphNodeParent.nbChild;
#endif
		}

		return 0;
	}

	void Entity::AddChild(Entity &_Child)
	{
		ASSERTM(m_hEntity, "Invalid Entity");
		ASSERTM(_Child, "Invalid Entity for _Child");

		if (m_hEntity)
		{
			auto &graphNodeParent = m_hEntity.get<Component_GraphNode>();

#ifdef ALLOW_DYNAMIC_CHILDREN
			if (graphNodeParent.firstChild != entt::null)
			{
				auto &reg = m_hEntity.registry();
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
				graphNodeParent.children[graphNodeParent.nbChild++] = _Child.m_hEntity.entity();
			}
#endif
		}
	}

	void Entity::RemoveChild(Entity &_Child)
	{
		ASSERTM(m_hEntity, "Invalid Entity");
		ASSERTM(_Child, "Invalid Entity for _Child");

		if (m_hEntity)
		{
			auto &graphNodeParent = m_hEntity.get<Component_GraphNode>();

#ifdef ALLOW_DYNAMIC_CHILDREN

			auto &reg = m_hEntity.registry();
			const auto &graphNodeChild = reg.get<Component_GraphNode>(_Child);

			if (graphNodeParent.firstChild == _Child.m_hEntity.entity())
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
				if (graphNodeParent.children[i] == _Child.m_hEntity.entity())
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

	void Entity::EditorUI()
	{
		if (ImGui::CollapsingHeader("Node"))
		{
			bool bFlag = HasFlag(EntityFlag::NO_SERIALISATION);
			if (ImGui::Checkbox("NO_SERIALISATION", &bFlag))
				SetFlag(EntityFlag::NO_SERIALISATION, bFlag);

			bFlag = HasFlag(EntityFlag::HIDE);
			if (ImGui::Checkbox("HIDE", &bFlag))
				SetFlag(EntityFlag::HIDE, bFlag);
		}

		ComponentManager::EditorUI(m_hEntity.registry(), m_hEntity.entity());
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

	bool Entity::IsVisible() const
	{
		return !HasFlag(EntityFlag::HIDE);
	}

	void Entity::SetVisible(bool _bVisible)
	{
		SetFlag(EntityFlag::HIDE, !_bVisible);
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

		auto &registry = m_hEntity.registry();

		j["HintID"] = m_hEntity.entity();
		j["Name"] = GetComponent<Component_Name>().sName;
		j[JSON_GUID] = GetComponent<Component_Name>().guid;
		ChildVisitor([&j](Entity &_child) {
			if (!_child.HasFlag(EntityFlag::NO_SERIALISATION))
			{
				GUID guid = _child.GetComponent<Component_Name>().guid;
				j[JSON_CHILDREN].push_back(guid);
			}
		});

		ComponentManager::ToJson(registry, m_hEntity.entity(), j);

		return j;
	}

	void Entity::FromJson(const json &_jEntity)
	{
		ASSERTM(m_hEntity.registry().empty() || !m_hEntity.registry().empty(), "Invalid registry ?");

		GetComponent<Component_Name>().sName = _jEntity["Name"];

		if (_jEntity.contains("Children"))
		{
			for (auto &id : _jEntity["Children"])
			{
				LevelBase** ppLevel = m_hEntity.registry().try_ctx<LevelBase*>();
				if (ppLevel && *ppLevel)
				{
					Entity e = (*ppLevel)->GetEntityFromGUID(id);
					e.SetParent(*this);
				}
			}
		}

		if (_jEntity.contains("Components"))
		{
			//Debug::TODO();
			ComponentManager::FromJson(m_hEntity.registry(), m_hEntity.entity(), _jEntity["Components"]);
		}
	}

	void Entity::AfterLoad()
	{
		ComponentManager::AfterLoad(m_hEntity.registry(), m_hEntity.entity());
	}

#pragma endregion

#else
#pragma region CTORS
Entity::Entity(entt::null_t)
	: m_EntityId(entt::null)
{
	m_xLevel.reset();
}

Entity::Entity(entt::entity _entityID, const LevelBaseWeakPtr& _xLevel)
	: m_EntityId(_entityID)
	, m_xLevel(_xLevel)
{}

Entity::Entity(const Entity& _other) noexcept
{
	m_EntityId = _other.m_EntityId;
	m_xLevel = _other.m_xLevel;
}

Entity::Entity(Entity&& _other) noexcept
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

Entity& Entity::operator=(const Entity& _other)
{
	m_EntityId = _other.m_EntityId;
	m_xLevel = _other.m_xLevel;

	return *this;
}

bool Entity::operator==(const Entity& _other)
{
	return m_EntityId == _other.m_EntityId && (!m_xLevel.expired() && !_other.m_xLevel.expired() && m_xLevel.lock() == _other.m_xLevel.lock());
}

bool Entity::operator!=(const Entity& _other)
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
		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid(m_EntityId))
		{
			auto& graphNode = reg.get<Component_GraphNode>(m_EntityId);

			parent.m_EntityId = graphNode.parent;
		}
	}

	return parent;
}

void Entity::SetParent(Entity& _Parent)
{
	ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");

	auto& reg = m_xLevel.lock()->m_ENTTRegistry;
	if (reg.valid(m_EntityId))
	{
		auto& graphNode = reg.get<Component_GraphNode>(m_EntityId);

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

void Entity::GetChildren(std::vector<Entity>& _outvecChildren) const
{
	_outvecChildren.clear();

	if (m_EntityId != entt::null && !m_xLevel.expired())
	{
		auto& reg = m_xLevel.lock()->m_ENTTRegistry;
		if (reg.valid(m_EntityId))
		{
			auto& graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

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
void Entity::AddChild(Entity& _Child)
{
	ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");
	ASSERTM(_Child, "Invalid Entity for _Child");

	auto& reg = m_xLevel.lock()->m_ENTTRegistry;
	if (reg.valid(m_EntityId))
	{
		auto& graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

#ifdef ALLOW_DYNAMIC_CHILDREN
		if (graphNodeParent.firstChild != entt::null)
		{
			auto& graphNodeChild = reg.get<Component_GraphNode>(graphNodeParent.firstChild);
			graphNodeChild.prevSibling = _Child;

			auto& graphNodeNewChild = reg.get<Component_GraphNode>(_Child);
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

void Entity::RemoveChild(Entity& _Child)
{
	ASSERTM(m_EntityId != entt::null && !m_xLevel.expired(), "Invalid Entity");
	ASSERTM(_Child, "Invalid Entity for _Child");

	auto& reg = m_xLevel.lock()->m_ENTTRegistry;
	if (reg.valid(m_EntityId))
	{

		auto& graphNodeParent = reg.get<Component_GraphNode>(m_EntityId);

#ifdef ALLOW_DYNAMIC_CHILDREN

		const auto& graphNodeChild = reg.get<Component_GraphNode>(_Child);

		if (graphNodeParent.firstChild == _Child.m_EntityId)
		{
			graphNodeParent.firstChild = graphNodeChild.nextSibling;
		}
		else //if (graphNodeChild.prevSibling != entt::null)
		{
			auto& graphNodePrev = reg.get<Component_GraphNode>(graphNodeChild.prevSibling);
			graphNodePrev.nextSibling = graphNodeChild.nextSibling;
		}
		if (graphNodeChild.nextSibling != entt::null)
		{
			auto& graphNodeNext = reg.get<Component_GraphNode>(graphNodeChild.nextSibling);
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

void Entity::EditorUI()
{
	auto& registry = m_xLevel.lock()->m_ENTTRegistry;

	if (ImGui::CollapsingHeader("Node"))
	{
		bool bFlag = HasFlag(EntityFlag::NO_SERIALISATION);
		if (ImGui::Checkbox("NO_SERIALISATION", &bFlag))
			SetFlag(EntityFlag::NO_SERIALISATION, bFlag);
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
void to_json(json& j, const Entity& e)
{
	j = e.ToJson();
}

json Entity::ToJson() const
{
	json j; // = EditorLayer::GetJsonEntity( *this );


	LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
	auto& registry = xLevel->m_ENTTRegistry;

	j["HintID"] = m_EntityId;
	j["Name"] = GetComponent<Component_Name>().sName;
	ChildVisitor([&j] (Entity& _child)
	{
		if (!_child.HasFlag(EntityFlag::NO_SERIALISATION))
			j["Children"].push_back(_child.m_EntityId);
	});

	ComponentManager::ToJson(registry, m_EntityId, j);

	return j;
}

void Entity::FromJson(const json& _jEntity)
{
	GetComponent<Component_Name>().sName = _jEntity["Name"];

	LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
	if (_jEntity.contains("Children"))
	{
		ASSERTM(xLevel != nullptr, "No current level");
		for (auto &id : _jEntity["Children"])
		{
			Entity e = xLevel->GetEntityFromGUID(id);
			e.SetParent(*this);
		}
	}

	if (_jEntity.contains("Components"))
	{
		auto& registry = xLevel->m_ENTTRegistry;
		ComponentManager::FromJson(registry, m_EntityId, _jEntity["Components"]);
	}
}
#pragma endregion
#endif
}
