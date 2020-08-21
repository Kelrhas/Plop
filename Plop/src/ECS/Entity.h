#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt.hpp>

#include <Debug/Debug.h>
#include <ECS/Level.h>

namespace Plop
{
	class EditorLayer;
	class Entity
	{
		friend class EditorLayer;
	public:

		Entity() = default;
		Entity( entt::entity _entityID, const LevelWeakPtr& _xLevel );
		Entity( const Entity& _other ) = delete;
		Entity( Entity&& _other ) noexcept;

		operator bool() const;
		Entity& operator =( const Entity& _other );
		bool operator ==( const Entity& _other );


		template <class Comp, typename ...Args>
		Comp& AddComponent(Args...args)
		{
			ASSERT( !m_xLevel.expired(), "No scene associated with entity" );
			return m_xLevel.lock()->m_ENTTRegistry.emplace<Comp>( m_EntityId, std::forward<Args>( args )... );
		}

		template <class Comp, typename ...Args>
		void RemoveComponent()
		{
			ASSERT( !m_xLevel.expired(), "No scene associated with entity" );
			return m_xLevel.lock()->m_ENTTRegistry.remove<Comp>( m_EntityId);
		}

		template <class Comp>
		bool HasComponent()
		{
			if (m_xLevel.expired())
				return false;

			return m_xLevel.lock()->m_ENTTRegistry.has<Comp>( m_EntityId );
		}

		template <class Comp>
		Comp& GetComponent()
		{
			ASSERT( !m_xLevel.expired(), "No scene associated with entity" );
			ASSERT( HasComponent<Comp>(), "Entity does not have this component" );
			return m_xLevel.lock()->m_ENTTRegistry.get<Comp>( m_EntityId );
		}


		void ImGuiDraw();

	private:
		entt::entity	m_EntityId{ entt::null };
		LevelWeakPtr	m_xLevel;
	};
}
