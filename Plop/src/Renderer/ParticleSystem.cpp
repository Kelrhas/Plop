#include "Plop_pch.h"
#include "ParticleSystem.h"

#include <imgui.h>

#include <Renderer/Renderer.h>

namespace Plop
{
	/*ParticleDataLive& ParticleDataLive::operator=( const ParticleData& _other )
	{
		this = &_other;
		return *this;
	}*/

	ParticleSystem::ParticleSystem()
	{
		m_iMaxNumberParticles = 500;
		m_pParticles = NEW ParticleDataLive[m_iMaxNumberParticles];
	}


	ParticleSystem::~ParticleSystem()
	{
		delete[] m_pParticles;
	}

	void ParticleSystem::Spawn( const ParticleData& _data, int iNbParticle /*= 1*/ )
	{
		PROFILING_FUNCTION();

		for (int i = 0; i < m_iMaxNumberParticles; ++i)
		{
			if (!m_pParticles[i].bActive)
			{
				ParticleDataLive& p = m_pParticles[i];
				p = _data;

				p.fLifeTime = _data.fLifeTimeBase + rand.NextFloat01() * (_data.fLifeTimeVariationMax - _data.fLifeTimeVariationMin) + _data.fLifeTimeVariationMin;
				p.vSize = _data.vSizeStart;
				p.vPosition = _data.vPositionBase + rand.Next01<glm::vec3>() * (_data.vPositionVariationMax - _data.vPositionVariationMin) + _data.vPositionVariationMin;
				p.vSpeed = _data.vSpeedStart;
				p.vColor = _data.vColorStart;

				p.bActive = true;
				p.fLife = 0.f;
				p.fLifeRatio = 0.f;

				--iNbParticle;
			}

			// for now do not spawn new particles
			if (iNbParticle == 0)
				break;
		}
	}

	void ParticleSystem::Update( const TimeStep& _timeStep )
	{
		PROFILING_FUNCTION();

		float fDT = _timeStep.GetGameDeltaTime();

		int iNbActive = 0;
		for (int i = 0; i < m_iMaxNumberParticles; ++i)
		{
			if (m_pParticles[i].bActive)
			{
				ParticleDataLive& p = m_pParticles[i];

				p.fLife += fDT;
				p.fLife = std::min( p.fLife, p.fLifeTime );
				p.fLifeRatio = p.fLife / p.fLifeTime;

				if (p.fLifeRatio >= 1.f)
					p.bActive = false;
				else
				{
					p.vPosition += p.vSpeed * fDT;
					//if( p.bApplyGravity )

					p.vSpeed = p.fLifeRatio * (p.vSpeedEnd - p.vSpeedStart) + p.vSpeedStart;
					p.vSize = p.fLifeRatio * (p.vSizeEnd - p.vSizeStart) + p.vSizeStart;
					p.vColor = p.fLifeRatio * (p.vColorEnd - p.vColorStart) + p.vColorStart;

					Renderer2D::DrawQuad( p.vColor, p.vPosition, p.vSize );
				}

				if (p.bActive)
					++iNbActive;
			}
		}
	}
}
