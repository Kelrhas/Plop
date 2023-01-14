#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/handle.hpp>
#include <entt/entity/fwd.hpp>

#include <Debug/Debug.h>
#include <ECS/LevelBase.h>
#include <ECS/EntityFlag.h>

namespace Plop
{
	class EditorLayer;
	class Entity
	{
		friend class EditorLayer;

#ifdef USE_ENTITY_HANDLE
		entt::handle		m_hEntity;
#else
		entt::entity		m_EntityId{ entt::null };
		LevelBaseWeakPtr	m_xLevel;
#endif

		//// CTORS
	public:
		Entity() = default;
#ifdef USE_ENTITY_HANDLE
		Entity(entt::handle _hEntity);
		Entity(entt::entity _entityID, entt::registry& _reg);
#else
		Entity(entt::null_t);
		Entity(entt::entity _entityID, const LevelBaseWeakPtr& _xLevel);
#endif
		Entity( const Entity& _other ) noexcept;
		Entity( Entity&& _other ) noexcept;

		void												Reset();


		//// OPERATORS & ACCESS
	public:
		operator bool() const;
		Entity& operator =( const Entity& _other );
		bool operator ==( const Entity& _other );
		bool operator !=(const Entity& _other);
#ifdef USE_ENTITY_HANDLE
		operator entt::entity() const { return m_hEntity.entity(); }
		operator entt::handle() const { return m_hEntity; }
#else
		operator entt::entity() const { return m_EntityId; }
#endif
		bool												IsFromLevel(GUID _guidLevel) const;

		//// HIERARCHY
	public:
		Entity												GetParent() const;
		// TODO: make sure the parent is not one of the (recursive) child
		void												SetParent( Entity& _Parent );

		bool												HasChildren() const;
		void												GetChildren(std::vector<Entity> &_outvecChildren) const;
		size_t												GetChildrenCount() const;
		
		template<typename Visitor>
		void												ChildVisitor(Visitor &&visitor) const;

	private:
		void												AddChild( Entity& _Child );
		void												RemoveChild( Entity& _Child );



		//// COMPONENTS
	public:
		template <class Comp, typename ...Args>	Comp&		AddComponent( Args...args );
		template <class Comp, typename ...Args>	void		RemoveComponent();
		template <class Comp>					bool		HasComponent() const;
		template <class Comp>					Comp&		GetComponent() const;
		void												EditorUI();
		bool												HasComponent(const entt::id_type _compID) const;


		//// FLAGS
	public:
		void												AddFlag( EntityFlag _flag );
		void												RemoveFlag( EntityFlag _flag );
		void												SetFlag( EntityFlag _flag, bool _bSet );
		bool												HasFlag( EntityFlag _flag ) const;

		bool												IsVisible() const;
		void												SetVisible(bool _bVisible);


		//// SERIALIZATION
	public:
		virtual nlohmann::json								ToJson() const;
		virtual void										FromJson(const nlohmann::json& _jEntity );
		virtual void										AfterLoad();
	};
}


#include <ECS/Entity.inl>