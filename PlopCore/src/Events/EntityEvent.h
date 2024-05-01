#pragma once

#include <ECS/Entity.h>
#include <Events/Event.h>

namespace Plop
{
	class EntityEvent : public Event
	{
	public:
		EntityEvent(Entity _entity);
		virtual ~EntityEvent();

		Entity entity;
	};

	class EntityCreatedEvent : public EntityEvent
	{
	public:
		EntityCreatedEvent(Entity _entity) : EntityEvent(_entity) {}
		MACRO_EVENT_TYPE(EntityCreatedEvent);
	};

	class EntityDestroyedEvent : public EntityEvent
	{
	public:
		EntityDestroyedEvent(Entity _entity) : EntityEvent(_entity) {}
		MACRO_EVENT_TYPE(EntityDestroyedEvent);
	};

	class PrefabInstantiatedEvent : public EntityEvent
	{
	public:
		PrefabInstantiatedEvent(Entity _entity) : EntityEvent(_entity) {}
		MACRO_EVENT_TYPE(PrefabInstantiatedEvent);
	};
} // namespace Plop
