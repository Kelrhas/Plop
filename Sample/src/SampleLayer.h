#pragma once

#include <ApplicationLayer.h>
#include <Camera/Camera2DController.h>
#include <Renderer/Mesh.h>
#include <ECS/Components/Component_ParticleSystem.h>
#include <ECS/LevelBase.h>
#include <ECS/Entity.h>

//////////////////////////////////////////////////////////////////////////
// SampleLayer
class SampleLayer : public Plop::ApplicationLayer
{
public:
	virtual void OnRegistered() override;
	virtual void OnUnregistered() override {}
	virtual void OnUpdate(Plop::TimeStep& _timeStep) override;

	virtual uint8_t GetPriority() const override { return 10; }

private:
};

//////////////////////////////////////////////////////////////////////////
// SampleLayer2D
class SampleLayer2D : public Plop::ApplicationLayer
{
public:
	virtual void OnRegistered() override;
	virtual void OnUnregistered() override;
	virtual void OnUpdate(Plop::TimeStep& _timeStep) override;

	virtual uint8_t GetPriority() const override { return 20; }

private:
};

