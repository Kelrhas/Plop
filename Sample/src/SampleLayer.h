#pragma once

#include <ApplicationLayer.h>
#include <Camera/OrthographicCamera.h>
#include <Camera/Camera2DController.h>
#include <Renderer/Mesh.h>
#include <Renderer/ParticleSystem.h>

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
	Plop::OrthographicCamera* m_pCamera = nullptr;
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
	Plop::Camera2DController	m_CameraController;
	Plop::MeshPtr				m_xTowerMesh = nullptr;
	glm::vec2					m_vPlayerPos = glm::vec2( 0.f, 0.f );
	glm::vec2					m_vPlayerSpeed = glm::vec2( 0.f, 0.f );
	Plop::ParticleSystem		m_particles;
};

