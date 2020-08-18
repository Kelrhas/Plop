#include "Plop_pch.h"
#include "Level.h"

#include <ECS/Entity.h>
#include <ECS/BaseComponents.h>

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

	Entity Level::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, weak_from_this() };

		e.AddComponent<NameComponent>( _sName );
		e.AddComponent<TransfomComponent>();

		return e;
	}
}
