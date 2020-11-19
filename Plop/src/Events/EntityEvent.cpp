#include "Plop_pch.h"
#include "EntityEvent.h"


namespace Plop
{
	EntityEvent::EntityEvent( const Entity& _entity ) : Event(), entity( _entity )
	{}


	EntityEvent::~EntityEvent()
	{}
}
