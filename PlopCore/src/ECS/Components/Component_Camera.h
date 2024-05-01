#pragma once


#include <ECS/ComponentManager.h>
#include <Camera/Camera.h>

namespace Plop
{
	struct Component_Camera
	{
		CameraPtr xCamera = nullptr;


		void EditorUI();
		json ToJson() const;
		void FromJson( const json& _json );
		Component_Camera& operator=( const Component_Camera& _other );

	};
}
