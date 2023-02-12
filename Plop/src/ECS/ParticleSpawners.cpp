#include "Plop_pch.h"
#include "ParticleSpawners.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "Application.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/ECSHelper.h"
#include "Utils/JsonTypes.h"

namespace Plop::Particle
{
#pragma region Spawners

	/* SpawnLife */
	void SpawnLife::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		_pParticle->fLifeTime = fLifetime + _system.GetRandom().NextFloatNeg11() * fVariation;
		_pParticle->fRemainingLife = _pParticle->fLifeTime;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnLife::Clone() const
	{
		SpawnLifePtr xSpawner = std::make_shared<SpawnLife>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnLife::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	void SpawnShapeCircle::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		float fAngle = _system.GetRandom().NextFloat01() * glm::two_pi<float>();

		glm::vec2 vUnitPos( glm::cos( fAngle ), glm::sin( fAngle ) );
		glm::vec3 res = glm::vec3( vUnitPos * fRadius, 0.f );
		_pParticle->vPosition += res;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnShapeCircle::Clone() const
	{
		SpawnShapeCirclePtr xSpawner = std::make_shared<SpawnShapeCircle>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeCircle::Editor(Entity _owner, const Component_ParticleSystem &_system)
	{
		ImGui::DragFloat("Radius", &fRadius, 0.1f, 0.f, std::numeric_limits<float>::infinity());

		glm::vec3 vPos = _owner.GetComponent<Component_Transform>().GetWorldPosition();
		EditorGizmo::Circle(vPos, fRadius);
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
	void SpawnShapeDisk::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		float fAngle = _system.GetRandom().NextFloat01() * glm::two_pi<float>();

		glm::vec2 vUnitPos( glm::cos( fAngle ), glm::sin( fAngle ) );
		glm::vec3 res = glm::vec3( vUnitPos * fRadius * _system.GetRandom().NextFloat01(), 0.f );
		_pParticle->vPosition += res;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnShapeDisk::Clone() const
	{
		SpawnShapeDiskPtr xSpawner = std::make_shared<SpawnShapeDisk>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeDisk::Editor(Entity _owner, const Component_ParticleSystem &_system)
	{
		ImGui::DragFloat("Radius", &fRadius, 0.1f, 0.f, std::numeric_limits<float>::infinity());

		glm::vec3 vPos = _owner.GetComponent<Component_Transform>().GetWorldPosition();
		EditorGizmo::Circle(vPos, fRadius);
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


	/* SpawnShapeArc */
	void SpawnShapeArc::Spawn(ParticleData *_pParticle, Component_ParticleSystem &_system)
	{
		const float fHalfAngleRad = glm::radians(fAngle);
		float		fAngleSpawn	  = _system.GetRandom().NextFloatNeg11() * fHalfAngleRad;

		glm::vec2 vUnitPos(glm::cos(fAngleSpawn), glm::sin(fAngleSpawn));
		glm::vec3 res = glm::vec3(vUnitPos * fRadius * _system.GetRandom().NextFloat01(), 0.f);

		Entity owner = GetComponentOwner(Application::GetCurrentLevel().lock()->GetEntityRegistry(), _system);
		const glm::mat4 mWorld = owner.GetComponent<Component_Transform>().GetWorldMatrix();

		res = mWorld * glm::vec4(res, 0.f);
		_pParticle->vPosition += res;
	}
	 
	Component_ParticleSystem::ParticleSpawnerPtr SpawnShapeArc::Clone() const
	{
		SpawnShapeArcPtr xSpawner = std::make_shared<SpawnShapeArc>();
		*xSpawner				   = *this;
		return xSpawner;
	}

	void SpawnShapeArc::Editor(Entity _owner, const Component_ParticleSystem &_system)
	{
		ImGui::DragFloat("Radius", &fRadius, 0.1f, 0.f, std::numeric_limits<float>::infinity());
		ImGui::SliderFloat("Angle (half arc)", &fAngle, 0.f, 180.f);
		ImGui::DragFloat2("Direction", &vDir.x, 0.1f, -1.f, 1.f);

		const float		fHalfAngleRad = glm::radians(fAngle);
		const glm::mat4 mWorld		  = _owner.GetComponent<Component_Transform>().GetWorldMatrix();
		const glm::vec3 &vPos = mWorld[3];

		const float fZAngle = glm::atan(mWorld[0][1], mWorld[0][0]);
		const float fScaledRadius = glm::abs(glm::length(mWorld[0]) * fRadius);
		// TODO take local scale into account, just for edition purpose, the particles are spawned correctly
		EditorGizmo::Arc(vPos, fScaledRadius, -fZAngle + fHalfAngleRad, -fZAngle - fHalfAngleRad);

		EditorGizmo::Line(vPos, vPos + (mWorld * glm::vec4(VEC3_RIGHT, 0.f)).xyz() * fRadius, COLOR_BLUE);
		EditorGizmo::Line(vPos, vPos + (glm::rotate(mWorld, -fHalfAngleRad, VEC3_Z) * glm::vec4(VEC3_RIGHT * fRadius, 0.f)).xyz());
		EditorGizmo::Line(vPos, vPos + (glm::rotate(mWorld, +fHalfAngleRad, VEC3_Z) * glm::vec4(VEC3_RIGHT * fRadius, 0.f)).xyz());
	}

	json SpawnShapeArc::to_json()
	{
		json j;
		j["Radius"] = fRadius;
		j["Angle"] = fAngle;
		j["Direction"] = vDir;
		return j;
	}

	void SpawnShapeArc::from_json(const json &_j)
	{
		if (_j.contains("Radius"))
			fRadius = _j.at("Radius");
		if (_j.contains("Angle"))
			fAngle = _j.at("Angle");
		if (_j.contains("Direction"))
			vDir = _j.at("Direction");
	}



	/* SpawnShapeRect */
	void SpawnShapeRect::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		float fx = _system.GetRandom().NextFloatNeg11() * vSize.x * 0.5f;
		float fy = _system.GetRandom().NextFloatNeg11() * vSize.y * 0.5f;

		glm::vec3 res = glm::vec3( fx, fy, 0.f );
		_pParticle->vPosition += res;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnShapeRect::Clone() const
	{
		SpawnShapeRectPtr xSpawner = std::make_shared<SpawnShapeRect>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnShapeRect::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	void SpawnColor::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		_pParticle->vColor = vColor;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnColor::Clone() const
	{
		SpawnColorPtr xSpawner = std::make_shared<SpawnColor>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnColor::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	void SpawnSize::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		_pParticle->vSize = vSize;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnSize::Clone() const
	{
		SpawnSizePtr xSpawner = std::make_shared<SpawnSize>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnSize::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	void SpawnRadialSpeed::Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system )
	{
		auto owner = GetComponentOwner(Application::GetCurrentLevel().lock()->GetEntityRegistry(), _system);
		glm::vec3 vCenter = owner ? owner.GetComponent<Component_Transform>().GetWorldPosition() : VEC3_0;

		_pParticle->vSpeed = glm::normalize( _pParticle->vPosition - vCenter ) * fSpeed;
	}

	Component_ParticleSystem::ParticleSpawnerPtr SpawnRadialSpeed::Clone() const
	{
		SpawnRadialSpeedPtr xSpawner = std::make_shared<SpawnRadialSpeed>();
		*xSpawner = *this;
		return xSpawner;
	}

	void SpawnRadialSpeed::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	void UpdatePositionFromSpeed::Update( ParticleData* _pParticle, float _fDeltaTime)
	{
		_pParticle->vPosition += _pParticle->vSpeed * _fDeltaTime;
		_pParticle->vSpeed *= 1.f - (fAttenuation * _fDeltaTime);
	}

	Component_ParticleSystem::ParticleUpdaterPtr UpdatePositionFromSpeed::Clone() const
	{
		UpdatePositionFromSpeedPtr xUpdater = std::make_shared<UpdatePositionFromSpeed>();
		*xUpdater = *this;
		return xUpdater;
	}

	void UpdatePositionFromSpeed::Editor(Entity _owner, const Component_ParticleSystem &_system)
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
	

	/* UpdateColorFromLifetime */
	void UpdateColorFromLifetime::Update(ParticleData *_pParticle, float _fDeltaTime)
	{
		_pParticle->vColor = glm::lerp(vColorStart, vColorEnd, _pParticle->fLifeRatio);
	}

	Component_ParticleSystem::ParticleUpdaterPtr UpdateColorFromLifetime::Clone() const
	{
		UpdateColorFromLifetimePtr xUpdater = std::make_shared<UpdateColorFromLifetime>();
		*xUpdater = *this;
		return xUpdater;
	}

	void UpdateColorFromLifetime::Editor(Entity _owner, const Component_ParticleSystem &_system)
	{
		ImGui::ColorEdit4( "Color start", &vColorStart.r, ImGuiColorEditFlags_None);
		ImGui::ColorEdit4( "Color end", &vColorEnd.r, ImGuiColorEditFlags_None);
	}

	json UpdateColorFromLifetime::to_json()
	{
		json j;
		j["ColorStart"] = vColorStart;
		j["ColorEnd"] = vColorEnd;
		return j;
	}

	void UpdateColorFromLifetime::from_json(const json &_j)
	{
		if (_j.contains( "ColorStart" ))
			vColorStart = _j.at( "ColorStart" );
		if (_j.contains( "ColorEnd" ))
			vColorEnd = _j.at( "ColorEnd" );
	}

#pragma endregion
}

