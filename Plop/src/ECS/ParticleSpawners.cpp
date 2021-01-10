#include "Plop_pch.h"
#include "ParticleSpawners.h"

#include <glm/gtc/type_ptr.hpp>

#include "ECS/BaseComponents.h"
#include "ECS/ECSHelper.h"
#include "Utils/JsonTypes.h"

namespace Plop::Particle
{
#pragma region Spawners

	/* SpawnLife */
	void SpawnLife::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		_pParticle->fLifeTime = fLifetime + _system.GetRandom().NextFloatNeg11() * fVariation;
		_pParticle->fRemainingLife = _pParticle->fLifeTime;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnLife::Clone() const
	{
		SpawnLifePtr xSpawner = std::make_shared<SpawnLife>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnLife::Editor()
	{
		ImGui::DragFloat( "Lifetime", &fLifetime, 0.1f, 0.f, std::numeric_limits<float>::infinity() );
		ImGui::DragFloat( "Variation", &fVariation, 0.1f, 0.f, fLifetime );
	}

	json SpawnLife::to_json()
	{
		json j;
		j["Lifetime"] = fLifetime;
		j["Variation"] = fVariation;
		return j;
	}

	void SpawnLife::from_json( const json& _j )
	{
		if (_j.contains( "Lifetime" ))
			fLifetime = _j["Lifetime"];
		if (_j.contains( "Variation" ))
			fVariation = _j["Variation"];
	}



	/* SpawnShapeCircle */
	void SpawnShapeCircle::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		float fAngle = _system.GetRandom().NextFloat01() * glm::two_pi<float>();

		glm::vec2 vUnitPos( glm::cos( fAngle ), glm::sin( fAngle ) );
		glm::vec3 res = glm::vec3( vUnitPos * fRadius, 0.f );
		_pParticle->vPosition += res;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnShapeCircle::Clone() const
	{
		SpawnShapeCirclePtr xSpawner = std::make_shared<SpawnShapeCircle>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeCircle::Editor()
	{
		ImGui::DragFloat( "Radius", &fRadius, 0.1f, 0.f, std::numeric_limits<float>::infinity() );
	}

	json SpawnShapeCircle::to_json()
	{
		json j;
		j["Radius"] = fRadius;
		return j;
	}

	void SpawnShapeCircle::from_json( const json& _j )
	{
		if (_j.contains( "Radius" ))
			fRadius = _j.at( "Radius" );
	}



	/* SpawnShapeDisk */
	void SpawnShapeDisk::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		float fAngle = _system.GetRandom().NextFloat01() * glm::two_pi<float>();

		glm::vec2 vUnitPos( glm::cos( fAngle ), glm::sin( fAngle ) );
		glm::vec3 res = glm::vec3( vUnitPos * fRadius * _system.GetRandom().NextFloat01(), 0.f );
		_pParticle->vPosition += res;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnShapeDisk::Clone() const
	{
		SpawnShapeDiskPtr xSpawner = std::make_shared<SpawnShapeDisk>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeDisk::Editor()
	{
		ImGui::DragFloat( "Radius", &fRadius, 0.1f, 0.f, std::numeric_limits<float>::infinity() );
	}

	json SpawnShapeDisk::to_json()
	{
		json j;
		j["Radius"] = fRadius;
		return j;
	}

	void SpawnShapeDisk::from_json( const json& _j )
	{
		if (_j.contains( "Radius" ))
			fRadius = _j.at( "Radius" );
	}



	/* SpawnShapeRect */
	void SpawnShapeRect::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		float fx = _system.GetRandom().NextFloatNeg11() * vSize.x * 0.5f;
		float fy = _system.GetRandom().NextFloatNeg11() * vSize.y * 0.5f;

		glm::vec3 res = glm::vec3( fx, fy, 0.f );
		_pParticle->vPosition += res;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnShapeRect::Clone() const
	{
		SpawnShapeRectPtr xSpawner = std::make_shared<SpawnShapeRect>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeRect::Editor()
	{
		ImGui::DragFloat2( "Size", glm::value_ptr( vSize ), 0.1f, 0.f, std::numeric_limits<float>::infinity() );
	}

	json SpawnShapeRect::to_json()
	{
		json j;
		j["Size"] = vSize;
		return j;
	}

	void SpawnShapeRect::from_json( const json& _j )
	{
		if (_j.contains( "Size" ))
			vSize = _j.at( "Size" );
	}



	/* SpawnColor */
	void SpawnColor::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		_pParticle->vColor = vColor;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnColor::Clone() const
	{
		SpawnColorPtr xSpawner = std::make_shared<SpawnColor>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnColor::Editor()
	{
		ImGui::ColorEdit4( "Color", glm::value_ptr( vColor ), ImGuiColorEditFlags_NoInputs );
	}

	json SpawnColor::to_json()
	{
		json j;
		j["Color"] = vColor;
		return j;
	}

	void SpawnColor::from_json( const json& _j )
	{
		if (_j.contains( "Color" ))
			vColor = _j.at( "Color" );
	}



	/* SpawnSize */
	void SpawnSize::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		_pParticle->vSize = vSize;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnSize::Clone() const
	{
		SpawnSizePtr xSpawner = std::make_shared<SpawnSize>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnSize::Editor()
	{
		ImGui::DragFloat2( "Size", glm::value_ptr( vSize ), 0.1f, 0.f, std::numeric_limits<float>().infinity() );
	}

	json SpawnSize::to_json()
	{
		json j;
		j["Size"] = vSize;
		return j;
	}

	void SpawnSize::from_json( const json& _j )
	{
		if (_j.contains( "Size" ))
			vSize = _j.at( "Size" );
	}



	/* SpawnRadialSpeed */
	void SpawnRadialSpeed::Spawn( ParticleData* _pParticle, ParticleSystemComponent& _system )
	{
		auto& entity = GetComponentOwner( LevelBase::GetCurrentLevel().lock(), _system );
		glm::vec3 vCenter = entity ? entity.GetComponent<TransformComponent>().vPosition : VEC3_0;

		_pParticle->vSpeed = glm::normalize( _pParticle->vPosition - vCenter ) * fSpeed;
	}

	ParticleSystemComponent::ParticleSpawnerPtr SpawnRadialSpeed::Clone() const
	{
		SpawnRadialSpeedPtr xSpawner = std::make_shared<SpawnRadialSpeed>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnRadialSpeed::Editor()
	{
		ImGui::DragFloat( "Speed", &fSpeed, 0.1f );
	}

	json SpawnRadialSpeed::to_json()
	{
		json j;
		j["Speed"] = fSpeed;
		return j;
	}

	void SpawnRadialSpeed::from_json( const json& _j )
	{
		if (_j.contains( "Speed" ))
			fSpeed = _j.at( "Speed" );
	}

#pragma endregion

#pragma region Updaters

	/* UpdatePositionFromSpeed */
	void UpdatePositionFromSpeed::Update( ParticleData* _pParticle, const TimeStep& _ts )
	{
		float dt = _ts.GetGameDeltaTime();
		_pParticle->vPosition += _pParticle->vSpeed * dt;
		_pParticle->vSpeed *= 1.f - (fAttenuation * dt);
	}

	ParticleSystemComponent::ParticleUpdaterPtr UpdatePositionFromSpeed::Clone() const
	{
		UpdatePositionFromSpeedPtr xUpdater = std::make_shared<UpdatePositionFromSpeed>();
		*xUpdater = *this;
		return xUpdater;
	}

	void UpdatePositionFromSpeed::Editor()
	{
		ImGui::DragFloat( "Attenuation", &fAttenuation, 0.01f, 0.f, 1.f, "%.4f", ImGuiSliderFlags_None );
	}

	json UpdatePositionFromSpeed::to_json()
	{
		json j;
		j["Attenuation"] = fAttenuation;
		return j;
	}

	void UpdatePositionFromSpeed::from_json( const json& _j )
	{
		if (_j.contains( "Attenuation" ))
			fAttenuation = _j.at( "Attenuation" );
	}

#pragma endregion
}

