#pragma once

#include <entt/entity/helper.hpp>


namespace Plop
{
	template<typename Component>
	Entity GetComponentOwner( const LevelBasePtr& _xLevel, const Component& _comp )
	{
		return Entity( entt::to_entity( _xLevel->GetEntityRegistry(), _comp ), _xLevel );
	}

}