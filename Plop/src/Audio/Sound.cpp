#include "Plop_pch.h"
#include "Sound.h"

#include <fstream>

void Assert_AL()
{
#ifdef _DEBUG
	int err = alGetError();
	if (err != AL_NO_ERROR)
		BREAK();
#endif
}

namespace Plop
{

	Sound::~Sound()
	{
		alSourcei( m_uBufferID, AL_BUFFER, 0 );
		Assert_AL();
		alDeleteBuffers( 1, &m_uBufferID );
		Assert_AL();
	}

	void Sound::Reset()
	{
		if( m_pSamples )
		{
			alGenBuffers( 1, &m_uBufferID );
			Assert_AL();

			alBufferData( m_uBufferID, m_eFormat, &m_pSamples[0], m_uDataSize, m_uFrequency );
			Assert_AL();
		}
	}
}