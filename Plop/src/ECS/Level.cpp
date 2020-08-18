#include "Plop_pch.h"
#include "Level.h"

#include <ECS/Entity.h>

namespace Plop
{
	Level::~Level()
	{
	}

	void Level::Init()
	{
	}

	void Level::Shutdown()
	{
	}

	Entity Level::CreateEntity()
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, weak_from_this() };

		return e;
	}
}
