#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

#include <Debug/Debug.h>
#include <ECS/LevelBase.h>
#include <ECS/EntityFlag.h>

namespace Plop
{
	class EditorLayer;
	class Entity
	{
		friend class EditorLayer;

		entt::basic_handle<entt::registry> m_hEntity;

		//// CTORS
	public:
		Entity() = default;
		Entity(entt::handle _hEntity);
		Entity(entt::entity _entityID, entt::registry& _reg);
		Entity(entt::null_t);
		Entity( const Entity& _other ) noexcept;
		Entity( Entity&& _other ) noexcept;

		void						Reset();


		//// OPERATORS & ACCESS
	public:
									operator bool() const;
		Entity&						operator =( const Entity& _other );
		bool						operator ==( const Entity& _other );
		bool						operator !=(const Entity& _other);
									operator entt::entity() const { return m_hEntity.entity(); }
									operator entt::handle() const { return m_hEntity; }
		entt::registry&				GetRegistry() const { return *m_hEntity.registry(); }
		bool						IsFromLevel(GUID _guidLevel) const;

		//// HIERARCHY
	public:
		Entity						GetParent() const;
		// TODO: make sure the parent is not one of the (recursive) child
		void						SetParent( Entity _Parent );

		bool						HasChildren() const;
		void						GetChildren(std::vector<Entity> &_outvecChildren) const;
		size_t						GetChildrenCount() const;
		
		template<typename Visitor> requires VisitorConcept<Visitor, Entity>
		void						VisitChildren(Visitor &&visitor) const;
		template<typename Visitor> requires VisitorConcept<Visitor, Entity>
		void						VisitChildrenRecursive(Visitor &&visitor) const;

	private:
		void						AddChild( Entity _Child );
		void						RemoveChild( Entity _Child );



		//// COMPONENTS
	public:
		template <class Comp, typename ...Args>
		Comp&						AddComponent( Args...args );
		template <class Comp, typename ...Args>
		void						RemoveComponent();
		template <class Comp>
		bool						HasComponent() const;
		template <class Comp>
		Comp&						GetComponent() const;
		void						EditorUI();
		bool						HasComponent(const entt::id_type _compID) const;


		//// FLAGS
	public:
		void						AddFlag( EntityFlag _flag );
		void						RemoveFlag( EntityFlag _flag );
		void						SetFlag( EntityFlag _flag, bool _bSet );
		bool						HasFlag( EntityFlag _flag ) const;

		bool						IsVisible() const;
		void						SetVisible(bool _bVisible);


		//// SERIALIZATION
	public:
		virtual nlohmann::json		ToJson() const;
		virtual void				FromJson(const nlohmann::json& _jEntity );
		virtual void				AfterLoad();
	};

	//static_assert(std::is_trivially_copyable_v<Entity>);
}


#include <ECS/Entity.inl>