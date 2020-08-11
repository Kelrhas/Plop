#pragma once

#include <TimeStep.h>
#include <Utils/Random.h>

namespace Plop
{
	struct ParticleData
	{
#define PARTICLE_PROPERTY_VARIATION(type, name) type name##Base; \
												type name##VariationMin; \
												type name##VariationMax;

#define PARTICLE_PROPERTY_OVER_TIME(type, name) type name##Start; \
												type name##End;

#define PARTICLE_PROPERTY_VARIATION_OVER_TIME(type, name)	type name##StartBase; \
															type name##StartVariationMin; \
															type name##StartVariationMax; \
															type name##EndBase; \
															type name##EndVariationMin; \
															type name##EndVariationMax;

		PARTICLE_PROPERTY_VARIATION( float, fLifeTime )
		PARTICLE_PROPERTY_VARIATION( glm::vec3, vPosition )

		PARTICLE_PROPERTY_OVER_TIME( glm::vec2, vSize )
		PARTICLE_PROPERTY_OVER_TIME( glm::vec3, vSpeed )
		PARTICLE_PROPERTY_OVER_TIME( glm::vec4, vColor )

		// bool bApplyGravity = false;

		// FUTURE: Have automatic handling of property with macro
		// FUTURE: Have curve properties
	};

	// for spawned particles
	struct ParticleDataLive : public ParticleData
	{
		ParticleDataLive() = default;
		ParticleDataLive( const ParticleData& _other ) : ParticleData( _other ) {}

		// actual properties computed at spawn from Base + Variation
		float fLifeTime;
		glm::vec2 vSize;
		glm::vec3 vPosition;
		glm::vec3 vSpeed;
		glm::vec4 vColor;


		bool bActive = false;
		float fLife;
		float fLifeRatio;


		//ParticleDataLive& operator=( const ParticleData& _other );
	};

	class ParticleSystem
	{
	public:
		ParticleSystem();
		~ParticleSystem();

		void Spawn( const ParticleData& _data, int iNbParticle = 1 );
		void Update( const TimeStep& _timeStep );

		// FUTURE: write a proper Pool class
		ParticleDataLive*	m_pParticles = nullptr;
		int					m_iMaxNumberParticles;

	private:
		Random rand;
	};
}
