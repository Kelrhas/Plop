#pragma once

#include <Events/Event.h>
#include <Events/IEventListener.h>

namespace Plop
{
	class EventDispatcher
	{
	public:
		enum class Layer : uint8_t
		{
			NONE 	= 0x00,
			CORE 	= 0x01,
			INPUT 	= 0x02,
			GUI 	= 0xF0,
			ALL		= 0xFF
		};
	
		static void RegisterListener(IEventListener* _pListener, uint8_t _uLayer = (uint8_t)Layer::ALL);
		static void UnregisterListener(IEventListener* _pListener, uint8_t _uLayer = (uint8_t)Layer::ALL);
		static void SendEvent(Event& _event, uint8_t _uLayer = (uint8_t)Layer::ALL);
		static void Destroy();
		
	private:
		struct ListenerLayer
		{
			IEventListener* pListener;
			uint8_t uLayer;
		};
	
		static std::vector<ListenerLayer>		m_vecListeners;
	};
}