#include <Plop_pch.h>
#include "Random.h"

#include <time.h>

namespace Plop
{
	////////////////////
	/// Random
	////////////////////
	
	Random::Random(uint64_t _uSeed /*= 0*/)
	{
		SetSeed(_uSeed);
	}
	
	void Random::SetSeed(uint64_t _uSeed)
	{
		m_uSeed = _uSeed;
		
		if(m_uSeed == 0)
			m_uSeed = time(NULL);
		
		m_uState = m_uSeed;
	}
	
	uint32_t Random::NextInt()
	{
		uint64_t x = m_uState;
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
		m_uState = x;
		
		return (uint32_t)(x >> 32);
	}
	
	float Random::NextFloat01()
	{
		COMPILE_ASSERT( std::numeric_limits< double >::is_iec559, "Not using IEEE 754" );

		// we need to set only the mantissa
		uint32_t u = NextInt() & 0x007fffff | 0x3F800000; // setting sign = 0, exponent = 01111111 (2^0) -> yields float between 1 and 2
		float f;
		memcpy( &f, &u, sizeof(u) );
		return f - 1.f;
	}
	
	float Random::NextFloatNeg11()
	{
		COMPILE_ASSERT( std::numeric_limits< double >::is_iec559, "Not using IEEE 754" );

		// we need to set only the mantissa
		uint32_t u = NextInt() & 0x007fffff | 0x04000000; // setting sign = 0, exponent = 10000000 (2^1) -> yields float between 2 and 4
		float f;
		memcpy( &f, &u, sizeof( u ) );
		return f - 3.f;
	}
	
}