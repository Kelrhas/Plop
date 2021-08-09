#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/fwd.hpp>

#include <Debug/Debug.h>
#include <ECS/LevelBase.h>

namespace Plop
{
	class EditorLayer;
	class Entity
	{
		friend class EditorLayer;
	public:

		Entity() = default;
		Entity( entt::null_t );
		Entity( entt::entity _entityID, const LevelBaseWeakPtr& _xLevel );
		Entity( const Entity& _other ) noexcept;
		Entity( Entity&& _other ) noexcept;

		void												Reset();
		void												Destroy();

		operator bool() const;
		Entity& operator =( const Entity& _other );
		bool operator ==( const Entity& _other );
		bool operator !=( const Entity& _other );
		operator entt::entity() const { return m_EntityId; }

		Entity												GetParent() const;
		void												SetParent( Entity& _Parent );


		void												GetChildren(std::vector<Entity>& _outvecChildren ) const;
		
		template<typename Visitor>
		void												ChildVisitor( Visitor visitor ) const;


		template <class Comp, typename ...Args>	Comp&		AddComponent( Args...args );
		template <class Comp, typename ...Args>	void		RemoveComponent();
		template <class Comp>					bool		HasComponent() const;
		template <class Comp>					Comp&		GetComponent() const;


		virtual nlohmann::json								ToJson() const;
		virtual void										FromJson(const nlohmann::json& _jEntity );


	private:
		void												AddChild( Entity& _Child );
		void												RemoveChild( Entity& _Child );


		entt::entity	m_EntityId{ entt::null };
		LevelBaseWeakPtr	m_xLevel;
	};
}


#include <ECS/Entity.inl>