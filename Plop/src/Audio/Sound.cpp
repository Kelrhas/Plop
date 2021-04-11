#include "Plop_pch.h"
#include "Sound.h"

#include <fstream>

namespace Plop
{

	Sound::~Sound()
	{
		alDeleteBuffers( 1, &m_uBufferID );
		Debug::Assert_AL();
	}

	void Sound::Reset()
	{
		if( m_pSamples )
		{
			alGenBuffers( 1, &m_uBufferID );
			Debug::Assert_AL();

			alBufferData( m_uBufferID, m_eFormat, &m_pSamples[0], m_uDataSize, m_uFrequency );
			Debug::Assert_AL();
		}
	}
}