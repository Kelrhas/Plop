#include "Plop_pch.h"
#include "EventDispatcher.h"

namespace Plop
{
	std::vector<EventDispatcher::ListenerLayer> EventDispatcher::m_vecListeners;

	void EventDispatcher::RegisterListener(IEventListener* _pListener, uint8_t _uLayer /*= Layer::ALL*/)
	{
		for(ListenerLayer& listener : m_vecListeners)
		{
			if(listener.pListener == _pListener)
			{
				listener.uLayer = _uLayer;
				return;
			}
		}
		
		m_vecListeners.push_back({_pListener, _uLayer});
	}
	
	void EventDispatcher::UnregisterListener(IEventListener* _pListener, uint8_t _uLayer /*= Layer::ALL*/)
	{
		for (std::vector<ListenerLayer>::const_iterator it( m_vecListeners.begin()); it != m_vecListeners.end(); ++it)
		{
			const ListenerLayer& listener = *it;
			if(listener.pListener == _pListener)
			{
				m_vecListeners.erase(it);
				return;
			}
		}
	}

	void EventDispatcher::Destroy()
	{
		// make sure everyone has unregistered
		ASSERTM( m_vecListeners.size() == 0, "There are %d EventListener still registered", (int)m_vecListeners.size());
	}

}