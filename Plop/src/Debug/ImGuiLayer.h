#pragma once

#include <ApplicationLayer.h>

namespace Plop
{
	class ImGuiLayer : public ApplicationLayer
	{
	public:
		virtual void OnRegistered() override;
		virtual void OnUnregistered() override;
		virtual void OnUpdate(TimeStep& _timeStep) override;

		virtual uint8_t GetPriority() const { return 200; }


				void NewFrame();
				void EndFrame();
	};
}

