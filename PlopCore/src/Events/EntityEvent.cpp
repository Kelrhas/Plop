#include "Config.h"

#include "EntityEvent.h"


namespace Plop
{
	EntityEvent::EntityEvent( Entity _entity ) : Event(), entity( _entity )
	{}


	EntityEvent::~EntityEvent()
	{}
}
