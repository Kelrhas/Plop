#include "Config.h"

#include "SampleComponents.h"

#include "Utils/JsonTypes.h"

#include <glm/gtc/type_ptr.hpp>

void Component_Rotating::EditorUI()
{
	ImGui::DragFloat3("Axis", glm::value_ptr(vAxis), 0.1f);
	ImGui::DragFloat("Speed", &fSpeed, 0.1f, 0.01f);
}

void Component_Rotating::FromJson(const json &_j)
{
	if (_j.contains("Axis"))
		vAxis = _j["Axis"];
	if (_j.contains("Speed"))
		fSpeed = _j["Speed"];
}

json Component_Rotating::ToJson()
{
	json j;
	j["Axis"]  = vAxis;
	j["Speed"] = fSpeed;
	return j;
}