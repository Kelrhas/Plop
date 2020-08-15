#pragma once

#include <ApplicationLayer.h>
#include <Events/IEventListener.h>

namespace Plop
{
	class EditorLayer : public ApplicationLayer, IEventListener
	{
	public:
		virtual void OnRegistered() override;
		virtual void OnUnregistered() override;
		virtual void OnUpdate( TimeStep& _timeStep ) override;

		virtual bool OnEvent( Event& _event ) override;

		virtual uint8_t GetPriority() const { return 190; }


				void ShowMenuBar();
	};
}
