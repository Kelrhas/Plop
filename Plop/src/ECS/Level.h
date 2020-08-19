#pragma once

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt.hpp>


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

				Entity CreateEntity( const String& _sName = "New Entity" );


	private:
		entt::registry	m_ENTTRegistry;

		static LevelWeakPtr s_xCurrentLevel;
	};
}
