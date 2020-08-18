#pragma once

#include <entt.hpp>

//#include <ECS/Entity.h>

namespace Plop
{
	class Entity;

	class Level;
	using LevelPtr = std::shared_ptr<Level>;
	using LevelWeakPtr = std::weak_ptr<Level>;

	class Level : public std::enable_shared_from_this<Level>
	{
		friend class Entity;
	public:
		Level() = default;
		virtual ~Level();

				void Init();
				void Shutdown();

				Entity CreateEntity();

	private:
		entt::registry	m_ENTTRegistry;
	};
}
