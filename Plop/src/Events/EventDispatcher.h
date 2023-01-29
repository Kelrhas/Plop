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
			NONE 	= (1 << 0),
			CORE 	= (1 << 1),
			INPUT 	= (1 << 2),
			GAME	= (1 << 3),

			GUI 	= (1 << 7),
			ALL		= (uint8_t)-1
		};
	
		static void RegisterListener(IEventListener* _pListener, uint8_t _uLayer = (uint8_t)Layer::ALL);
		static void UnregisterListener(IEventListener* _pListener, uint8_t _uLayer = (uint8_t)Layer::ALL);
		static void Destroy();

		template<typename Event_t>
		static void SendEvent(Event_t _event, uint8_t _uLayer = (uint8_t)Layer::ALL)
		{
			for (ListenerLayer &listener : m_vecListeners)
			{
				if ((listener.uLayer & _uLayer) != 0)
					listener.pListener->OnEvent(&_event);
			}
		}
		
	private:
		struct ListenerLayer
		{
			IEventListener* pListener;
			uint8_t uLayer;
		};
	
		static std::vector<ListenerLayer>		m_vecListeners;
	};
}