#pragma once

#include "ECS/Components/Component_ParticleSystem.h"


namespace Plop::Particle
{
#pragma region Spawners
	struct SpawnLife final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnLife( float _fLifetime = 2.f ) : fLifetime( _fLifetime ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Lifetime"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		float fLifetime = 2.f;
		float fVariation = 0.f;
	};
	using SpawnLifePtr = std::shared_ptr<SpawnLife>;

	// spawn on a ring
	struct SpawnShapeCircle final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnShapeCircle( float _fRadius = 0.5f ) : fRadius( _fRadius ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Shape:Circle"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		float fRadius = 0.5f;
	};
	using SpawnShapeCirclePtr = std::shared_ptr<SpawnShapeCircle>;

	// spawn within a radius
	struct SpawnShapeDisk final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnShapeDisk( float _fRadius = 0.5f ) : fRadius( _fRadius ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Shape:Disk"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		float fRadius;
	};
	using SpawnShapeDiskPtr = std::shared_ptr<SpawnShapeDisk>;

	struct SpawnShapeArc final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnShapeArc() = default;
		SpawnShapeArc(float _fRadius, float _fAngle, const glm::vec2 &_vDir) : fRadius(_fRadius), fAngle(_fAngle), vDir(_vDir) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Shape:Arc"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		float fRadius = 0.5f;
		float fAngle = 45.f;
		glm::vec2 vDir;
	};
	using SpawnShapeArcPtr = std::shared_ptr<SpawnShapeArc>;

	struct SpawnShapeRect final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnShapeRect( const glm::vec2& _vSize = glm::vec2( 0.5f, 0.5f ) ) : vSize( _vSize ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Shape:Rect"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		glm::vec2 vSize;
	};
	using SpawnShapeRectPtr = std::shared_ptr<SpawnShapeRect>;

	struct SpawnColor final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnColor( const glm::vec4& _vColor = COLOR_WHITE ) : vColor( _vColor ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Color"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		glm::vec4 vColor;
	};
	using SpawnColorPtr = std::shared_ptr<SpawnColor>;

	struct SpawnSize final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnSize( const glm::vec2& _vSize = glm::vec2( 0.01f, 0.01f ) ) : vSize( _vSize ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Size"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		glm::vec2 vSize;
	};
	using SpawnSizePtr = std::shared_ptr<SpawnSize>;

	struct SpawnRadialSpeed final : public Component_ParticleSystem::ParticleSpawner
	{
		SpawnRadialSpeed( float _fSpeed = 1.f ) : fSpeed( _fSpeed ) {}
		virtual void Spawn( ParticleData* _pParticle, Component_ParticleSystem& _system ) override;
		virtual Component_ParticleSystem::ParticleSpawnerPtr Clone() const override;
		static const char* const StaticName() { return "Speed:Radial"; }
		virtual const char* const Name() override { return StaticName(); }
		virtual void Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json to_json() override;
		virtual void from_json( const json& )override;

		float fSpeed;
	};
	using SpawnRadialSpeedPtr = std::shared_ptr<SpawnRadialSpeed>;
#pragma endregion

#pragma region Updaters
	struct UpdatePositionFromSpeed final : public Component_ParticleSystem::ParticleUpdater
	{
		UpdatePositionFromSpeed( float _fAttenuation = 0.1f ) : fAttenuation( std::clamp( _fAttenuation, 0.f, 1.f ) ) {}
		virtual void 										 Update( ParticleData* _pParticle, float _fDeltaTime ) override;
		virtual Component_ParticleSystem::ParticleUpdaterPtr Clone() const override;
		static const char* const 							 StaticName() { return "Position from speed"; }
		virtual const char* const 							 Name() override { return StaticName(); }
		virtual void 										 Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json 										 to_json() override;
		virtual void 										 from_json( const json& )override;

		float fAttenuation;
	};
	using UpdatePositionFromSpeedPtr = std::shared_ptr<UpdatePositionFromSpeed>;

	struct UpdateColorFromLifetime final : public Component_ParticleSystem::ParticleUpdater
	{
		UpdateColorFromLifetime() {}
		virtual void										 Update(ParticleData *_pParticle, float _fDeltaTime) override;
		virtual Component_ParticleSystem::ParticleUpdaterPtr Clone() const override;
		static const char *const							 StaticName() { return "Color from lifetime"; }
		virtual const char *const							 Name() override { return StaticName(); }
		virtual void										 Editor(Entity _owner, const Component_ParticleSystem &_system) override;
		virtual json										 to_json() override;
		virtual void										 from_json(const json &) override;

		glm::vec4 vColorStart = COLOR_WHITE;
		glm::vec4 vColorEnd	  = COLOR_BLACK;
	};
	using UpdateColorFromLifetimePtr = std::shared_ptr<UpdateColorFromLifetime>;
#pragma endregion
}