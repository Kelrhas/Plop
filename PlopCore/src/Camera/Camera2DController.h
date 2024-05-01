#pragma once

#include <Events/IEventListener.h>
#include <Camera/Camera.h>
#include <TimeStep.h>

namespace Plop
{
	class Camera2DController : public IEventListener
	{
	public:

		// from IEventListener
		virtual bool OnEvent(Event *_pEvent) override;

				void Init();
				void OnUpdate(TimeStep& _timeStep);
				bool OnMouseWheel(float _fWheel);



				Camera& GetCamera() { return m_camera; }
				const Camera& GetCamera() const { return m_camera; }

	private:
				void SetSize();


		float m_fZoomLevel = 1.f;
		Camera m_camera;
	};
}
