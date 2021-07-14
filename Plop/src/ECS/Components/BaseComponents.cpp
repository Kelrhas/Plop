#include "Plop_pch.h"
#include "BaseComponents.h"


namespace MM
{
	template <>
	json ComponentToJson<Plop::Component_Name>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<Plop::Component_Name>( e );
		return json( comp.sName );
	}

}
