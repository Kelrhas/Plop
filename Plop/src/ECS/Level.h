#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt.hpp>

#include <TimeStep.h>

namespace Plop
{
	class Entity;

	class Level;
	using LevelPtr = std::shared_ptr<Level>;
	using LevelWeakPtr = std::weak_ptr<Level>;

	/**
	 * Hold entities
	 */
	class Level : public std::enable_shared_from_this<Level>
	{
		friend class Entity;
		friend class EditorLayer;
	public:
		Level() = default;
		virtual ~Level();

				void Init();
				void Shutdown();

				void Update( TimeStep _ts );

				Entity CreateEntity( const String& _sName = "New Entity" );


				entt::registry& GetEntityRegistry() { return m_ENTTRegistry; }
				const entt::registry& GetEntityRegistry() const { return m_ENTTRegistry; }


		static LevelWeakPtr GetCurrentLevel() { return s_xCurrentLevel; }


	protected:
		static LevelWeakPtr s_xCurrentLevel;

	private:
		entt::registry	m_ENTTRegistry;

	};
}
