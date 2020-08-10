#include "Plop_pch.h"
#include "Camera2DController.h"

#include <Application.h>
#include <Input/Input.h>

namespace Plop
{
	bool Camera2DController::OnEvent(Event& _event)
	{
		if (_event.GetEventType() == EventType::WindowSizeEvent)
			SetSize();

		return false;
	}

	void Camera2DController::Init()
	{
		SetSize();
		Input::RegisterMouseWheel([this](auto&& ...args) { return this->OnMouseWheel(std::forward<decltype(args)>(args)...); });
	}

	void Camera2DController::OnUpdate(TimeStep& _timeStep)
	{
		float fSpeed = m_fZoomLevel;

		if (Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Left))
			m_camera.Translate(-VEC3_RIGHT * _timeStep.GetGameDeltaTime() * fSpeed );
		if (Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Right))
			m_camera.Translate(VEC3_RIGHT * _timeStep.GetGameDeltaTime() * fSpeed );
		if (Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Up))
			m_camera.Translate(VEC3_UP * _timeStep.GetGameDeltaTime() * fSpeed );
		if (Plop::Input::IsKeyDown(Plop::KeyCode::KEY_Down))
			m_camera.Translate(-VEC3_UP * _timeStep.GetGameDeltaTime() * fSpeed );
	}

	bool Camera2DController::OnMouseWheel(float _fWheel)
	{
		m_fZoomLevel -= _fWheel;
		m_fZoomLevel = std::clamp(m_fZoomLevel, 0.2f, 10.f);
		SetSize();

		return false;
	}

	void Camera2DController::SetSize()
	{
		float fAspectRatio = Application::Get()->GetWindow().GetAspectRatio();
		m_camera.SetOrtho(-m_fZoomLevel * fAspectRatio, m_fZoomLevel * fAspectRatio, -m_fZoomLevel, m_fZoomLevel);
	}
}
