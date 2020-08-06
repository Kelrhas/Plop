#pragma once

#include <Events/IEventListener.h>
#include <Camera/OrthographicCamera.h>
#include <TimeStep.h>

namespace Plop
{
	class Camera2DController : public IEventListener
	{
	public:

		// from IEventListener
		virtual bool OnEvent(Event& _event) override;

				void Init();
				void OnUpdate(TimeStep& _timeStep);
				bool OnMouseWheel(float _fWheel);



				OrthographicCamera& GetCamera() { return m_camera; }
				const OrthographicCamera& GetCamera() const { return m_camera; }

	private:
				void SetSize();


		float m_fZoomLevel = 1.f;
		OrthographicCamera m_camera;
	};
}
