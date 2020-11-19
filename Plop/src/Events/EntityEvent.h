#pragma once

#include <Events/Event.h>
#include <ECS/Entity.h>

namespace Plop
{
	class EntityEvent : public Event
	{
	public:
		EntityEvent( const Entity& _entity );
		virtual ~EntityEvent();

		const Entity& entity;
	};

	class EntityDestroyedEvent : public EntityEvent
	{
	public:
		EntityDestroyedEvent( const Entity& _entity ) : EntityEvent( _entity ) {}
		MACRO_EVENT_TYPE( EntityDestroyedEvent );
	};
}

