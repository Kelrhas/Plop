#pragma once

#include <entt/entity/helper.hpp>
#include "ECS/Entity.h"
#include "ECS/LevelBase.h"


namespace Plop
{
	template<typename Component>
	Entity GetComponentOwner( const LevelBasePtr& _xLevel, const Component& _comp )
	{
		return Entity( entt::to_entity( _xLevel->GetEntityRegistry(), _comp ), _xLevel );
	}

}