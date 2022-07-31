#pragma once

#include <Events/Event.h>

namespace Plop
{
	class GameEvent : public Event
	{
		struct Data
		{
			union
			{
				void *pointer = nullptr;
				float f;
				int	  i;
			};
		};

	public:
		GameEvent(S32 _iType) : m_iGameEventType(_iType) {}

		MACRO_EVENT_TYPE(GameEvent);

		// does not manage memory
		void SetData(void *_pData) { m_Data.pointer = _pData; }
		void SetData(float _fData) { m_Data.f = _fData; }
		void SetData(int _iData) { m_Data.i = _iData; }

		// corresponds to whatever, ususally an enum
		S32	 m_iGameEventType;
		Data m_Data;
	};
} // namespace Plop
