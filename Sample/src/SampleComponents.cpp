#include <PlopInclude.h>
#include "SampleComponents.h"

#include <glm/gtc/type_ptr.hpp>

#include "Utils/JsonTypes.h"

namespace MM
{
	template <>
	void ComponentEditorWidget<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<RotatingComponent>( e );
		ImGui::DragFloat3( "Axis", glm::value_ptr( comp.vAxis ), 0.1f );
		ImGui::DragFloat( "Speed", &comp.fSpeed, 0.1f, 0.01f );
	}

	template <>
	json ComponentToJson<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e )
	{
		auto& comp = reg.get<RotatingComponent>( e );
		json j;
		j["Axis"] = comp.vAxis;
		j["Speed"] = comp.fSpeed;
		return j;
	}

	template<>
	void ComponentFromJson<RotatingComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j )
	{
		auto& comp = reg.get_or_emplace<RotatingComponent>( e );
		if (_j.contains( "Axis" ))
			comp.vAxis = _j["Axis"];
		if (_j.contains( "Speed" ))
			comp.fSpeed = _j["Speed"];
	}

}