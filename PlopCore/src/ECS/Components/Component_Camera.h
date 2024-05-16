#pragma once


#include <Camera/Camera.h>
#include <ECS/ComponentManager.h>

namespace Plop
{
	struct Component_Camera
	{
		Component_Camera() = default;
		Component_Camera(const Component_Camera &_other);
		Component_Camera(Component_Camera &&) = delete;
		Component_Camera &operator=(const Component_Camera &_other);
		Component_Camera &operator=(Component_Camera &&) = delete;

		void OnCreate(entt::registry &, entt::entity);
		void OnDestroy(entt::registry &, entt::entity);
		void EditorUI();
		json ToJson() const;
		void FromJson(const json &_json);

		CameraPtr GetCamera() const { return m_xCamera; }

	private:
		CameraPtr m_xCamera = nullptr;
	};
} // namespace Plop
