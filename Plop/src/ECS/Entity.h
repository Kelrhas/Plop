#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
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

		entt::entity		m_EntityId{ entt::null };
		LevelBaseWeakPtr	m_xLevel;

		//// CTORS
	public:
		Entity() = default;
		Entity( entt::null_t );
		Entity( entt::entity _entityID, const LevelBaseWeakPtr& _xLevel );
		Entity( const Entity& _other ) noexcept;
		Entity( Entity&& _other ) noexcept;

		void												Reset();
		void												Destroy();


		//// OPERATORS
	public:
		operator bool() const;
		Entity& operator =( const Entity& _other );
		bool operator ==( const Entity& _other );
		bool operator !=( const Entity& _other );
		operator entt::entity() const { return m_EntityId; }


		//// HIERARCHY
	public:
		Entity												GetParent() const;
		void												SetParent( Entity& _Parent );

		void												GetChildren(std::vector<Entity>& _outvecChildren ) const;
		
		template<typename Visitor>
		void												ChildVisitor( Visitor visitor ) const;

	private:
		void												AddChild( Entity& _Child );
		void												RemoveChild( Entity& _Child );



		//// COMPONENTS
	public:
		template <class Comp, typename ...Args>	Comp&		AddComponent( Args...args );
		template <class Comp, typename ...Args>	void		RemoveComponent();
		template <class Comp>					bool		HasComponent() const;
		template <class Comp>					Comp&		GetComponent() const;
		template <class Comp, class Registry>	void		EditorUIComponent(const char* _pComponentName, Registry& registry );
		void												EditorUI();

		static std::map<String, std::function<void( Entity )>>	s_mapAddComponent;


		//// FLAGS
		void												AddFlag( EntityFlag _flag );
		void												RemoveFlag( EntityFlag _flag );
		void												SetFlag( EntityFlag _flag, bool _bSet );
		bool												HasFlag( EntityFlag _flag ) const;



		//// SERIALIZATION
		virtual nlohmann::json								ToJson() const;
		virtual void										FromJson(const nlohmann::json& _jEntity );



	};
}


#include <ECS/Entity.inl>