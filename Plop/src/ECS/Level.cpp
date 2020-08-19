#include "Plop_pch.h"
#include "Level.h"

#include <ECS/Entity.h>
#include <ECS/BaseComponents.h>

namespace Plop
{
	LevelWeakPtr Level::s_xCurrentLevel;

	Level::~Level()
	{
	}

	void Level::Init()
	{
		s_xCurrentLevel = weak_from_this();
	}

	void Level::Shutdown()
	{
	}

	Entity Level::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, weak_from_this() };

		e.AddComponent<NameComponent>( _sName );
		e.AddComponent<TransformComponent>();

		return e;
	}
}
