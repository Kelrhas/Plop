#pragma once

#include "Constants.h"
#include "ECS/Entity.h"
#include "TimeStep.h"
#include "Utils/Random.h"

#include <ECS/ComponentManager.h>

namespace Plop
{
	struct ParticleData
	{
		float	  fLifeTime		 = 0.f;
		float	  fRemainingLife = -1.f;
		float	  fLifeRatio	 = 0.f;
		glm::vec3 vPosition		 = VEC3_0;
		glm::vec2 vSize			 = VEC2_1;
		glm::vec4 vColor		 = COLOR_WHITE;
		glm::vec3 vSpeed		 = VEC3_0;
	};

	class Component_ParticleSystem final
	{
	public:
		struct ParticleSpawner;
		struct ParticleUpdater;
		using ParticleSpawnerPtr = std::shared_ptr<ParticleSpawner>;
		using ParticleUpdaterPtr = std::shared_ptr<ParticleUpdater>;
		struct ParticleSpawner
		{
			virtual ~ParticleSpawner() {}
			virtual void			   Spawn(ParticleData *, Component_ParticleSystem &)			  = 0;
			virtual ParticleSpawnerPtr Clone() const												  = 0;
			virtual const char *const  Name()														  = 0;
			virtual void			   Editor(Entity _owner, const Component_ParticleSystem &_system) = 0;
			virtual json			   to_json()													  = 0;
			virtual void			   from_json(const json &)										  = 0;
		};

		struct ParticleUpdater
		{
			virtual ~ParticleUpdater() {}
			virtual void			   Update(ParticleData *, float _fDeltaTime)					  = 0;
			virtual ParticleUpdaterPtr Clone() const												  = 0;
			virtual const char *const  Name()														  = 0;
			virtual void			   Editor(Entity _owner, const Component_ParticleSystem &_system) = 0;
			virtual json			   to_json()													  = 0;
			virtual void			   from_json(const json &)										  = 0;
		};


		Component_ParticleSystem(size_t _iMaxParticles = 500);
		Component_ParticleSystem(const Component_ParticleSystem &) noexcept;
		Component_ParticleSystem(Component_ParticleSystem &&) noexcept;
		~Component_ParticleSystem();

		Component_ParticleSystem &operator=(const Component_ParticleSystem &) noexcept;
		Component_ParticleSystem &operator=(Component_ParticleSystem &&) noexcept;

		void Spawn(size_t iNbParticle = 1);
		void Update(const TimeStep &_ts);
		void ResetSystem();

		void AddSpawner(const ParticleSpawnerPtr &_xSpawner);
		void AddUpdater(const ParticleUpdaterPtr &_xUpdater);
		void Clear();

		void  SetAutoSpawnRate(float _fSpawnRate) { m_fAutoSpawnRate = _fSpawnRate; } // number per seconds
		float GetAutoSpawnRate() const { return m_fAutoSpawnRate; }					  // number per seconds

		void			 SetMaxNbParticles(size_t _iMaxParticles);
		size_t			 GetMaxNbParticles() const { return m_iParticleCapacity; }
		size_t			 GetNbActiveParticles() const { return m_iActiveParticleCount; }
		void			 SetBasePosition(const glm::vec3 &_vPosition) { m_vBasePosition = _vPosition; }
		glm::vec3		 GetBasePosition() const { return m_vBasePosition; }
		const glm::mat4 &GetCachedMatrix() const { return m_mCachedWorld; }

		Random								  &GetRandom() { return m_rand; }
		std::vector<ParticleSpawnerPtr>		  &GetSpawners() { return m_vecSpawners; }
		const std::vector<ParticleSpawnerPtr> &GetSpawners() const { return m_vecSpawners; }
		std::vector<ParticleUpdaterPtr>		  &GetUpdaters() { return m_vecUpdaters; }
		const std::vector<ParticleUpdaterPtr> &GetUpdaters() const { return m_vecUpdaters; }


		void EditorUI();
		json ToJson() const;
		void FromJson(const json &_j);

	private:
		// TODO: write a proper Pool class
		// TODO: convert to SoA instead of AoS
		// TODO: go full GPU side
		ParticleData				   *m_pParticles		   = nullptr;
		size_t							m_iParticleCapacity	   = 0;
		size_t							m_iActiveParticleCount = 0;
		bool							m_bBurst			   = false;
		bool							m_bDestroyAfterBurst   = true;
		float							m_fAutoSpawnRate = 0.f, m_fAutoSpawnRemainder = 0.f;
		Random							m_rand;
		std::vector<ParticleSpawnerPtr> m_vecSpawners;
		std::vector<ParticleUpdaterPtr> m_vecUpdaters;
		glm::vec3						m_vBasePosition = VEC3_0;
		glm::mat4						m_mCachedWorld;
	};

} // namespace Plop
