#pragma once

#include <entt/entt.hpp>
#include <imgui_entt_entity_editor.hpp>

#include "Constants.h"
#include "TimeStep.h"
#include "Utils/Random.h"

namespace Plop
{
	struct ParticleData
	{
		float fLifeTime = 0.f;
		float fRemainingLife = -1.f;
		float fLifeRatio = 0.f;
		glm::vec3 vPosition = VEC3_0;
		glm::vec2 vSize = VEC2_1;
		glm::vec4 vColor = COLOR_WHITE;
		glm::vec3 vSpeed = VEC3_0;
	};

	class ParticleSystemComponent final
	{
	public:
		struct ParticleSpawner;
		struct ParticleUpdater;
		using ParticleSpawnerPtr = std::shared_ptr<ParticleSpawner>;
		using ParticleUpdaterPtr = std::shared_ptr<ParticleUpdater>;
		struct ParticleSpawner
		{
			virtual ~ParticleSpawner() {}
			virtual void Spawn( ParticleData*, ParticleSystemComponent& ) = 0;
			virtual ParticleSpawnerPtr Clone() const = 0;
			virtual const char* const Name() = 0;
			virtual void Editor() = 0;
			virtual json to_json() = 0;
			virtual void from_json( const json& ) = 0;
		};

		struct ParticleUpdater
		{
			virtual ~ParticleUpdater() {}
			virtual void Update( ParticleData*, const TimeStep& ) = 0;
			virtual ParticleUpdaterPtr Clone() const = 0;
			virtual const char* const Name() = 0;
			virtual void Editor() = 0;
			virtual json to_json() = 0;
			virtual void from_json( const json& ) = 0;
		};


		ParticleSystemComponent(size_t _iMaxParticles = 500);
		ParticleSystemComponent( const ParticleSystemComponent&) noexcept;
		ParticleSystemComponent( ParticleSystemComponent&&) noexcept;
		~ParticleSystemComponent();

		ParticleSystemComponent& operator=( const ParticleSystemComponent& ) noexcept;
		ParticleSystemComponent& operator=( ParticleSystemComponent&& ) noexcept;

		void Spawn( size_t iNbParticle = 1 );
		void Update( const TimeStep& _ts );
		void ResetSystem();

		void AddSpawner( const ParticleSpawnerPtr& _xSpawner );
		void AddUpdater( const ParticleUpdaterPtr& _xUpdater );
		void Clear();

		void SetAutoSpawnRate( float _fSpawnRate ) { m_fAutoSpawnRate = _fSpawnRate; } // number per seconds
		float GetAutoSpawnRate() const { return m_fAutoSpawnRate; } // number per seconds

		void SetMaxNbParticles( size_t _iMaxParticles );
		size_t GetMaxNbParticles() const { return m_iMaxNumberParticles; }
		size_t GetNbActiveParticles() const { return m_iNbActiveParticles; }
		void SetBasePosition( const glm::vec3& _vPosition ) { m_vBasePosition = _vPosition; }
		glm::vec3 GetBasePosition() const { return m_vBasePosition; }

		Random& GetRandom() { return m_rand; }
		std::vector<ParticleSpawnerPtr>& GetSpawners() { return m_vecSpawners; }
		std::vector<ParticleUpdaterPtr>& GetUpdaters() { return m_vecUpdaters; }



	private:
		// TODO: write a proper Pool class
		// TODO: convert to SoA instead of AoS
		// TODO: go full GPU side
		ParticleData*					m_pParticles = nullptr;
		size_t							m_iMaxNumberParticles = 0;
		size_t							m_iNbActiveParticles = 0;
		float							m_fAutoSpawnRate = 0.f, m_fAutoSpawnRemainder = 0.f;
		Random							m_rand;
		std::vector<ParticleSpawnerPtr> m_vecSpawners;
		std::vector<ParticleUpdaterPtr> m_vecUpdaters;
		glm::vec3						m_vBasePosition = VEC3_0;
	};

}



namespace MM
{
	template <>	void ComponentEditorWidget<Plop::ParticleSystemComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Plop::ParticleSystemComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Plop::ParticleSystemComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );

}
