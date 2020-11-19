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
		Entity( entt::entity _entityID, const LevelBaseWeakPtr& _xLevel );
		Entity( const Entity& _other ) = delete;
		Entity( Entity&& _other ) noexcept;

		void												Reset();

		operator bool() const;
		Entity& operator =( const Entity& _other );
		bool operator ==( const Entity& _other );
		bool operator !=( const Entity& _other );
		operator entt::entity() { return m_EntityId; }

		Entity												GetParent() const;
		void												SetParent( Entity& _Parent );

		std::vector<Entity>									GetChildren() const;

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