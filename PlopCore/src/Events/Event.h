#pragma once

#include "Types.h"

namespace Plop
{
	// contains every type of event
	enum class EventType : U32
	{
		None,
		WindowCloseEvent,
		WindowMoveEvent,
		WindowSizeEvent,

		EntityCreatedEvent,
		EntityDestroyedEvent,
		PrefabInstantiatedEvent,

		GameEvent
	};

#define MACRO_EVENT_TYPE(type)                                           \
	static EventType  GetStaticEventType() { return EventType::##type; } \
	virtual EventType GetEventType() const override { return GetStaticEventType(); }

	class Event
	{
	public:
		virtual ~Event() {}

		virtual EventType GetEventType() const = 0; // use MACRO_EVENT_TYPE if missing

		bool IsHandled() const { return m_bHandled; }

	private:
		bool m_bHandled = false;
	};
} // namespace Plop