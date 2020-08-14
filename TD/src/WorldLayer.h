#pragma once

#include <ApplicationLayer.h>
#include <Camera/OrthographicCamera.h>
#include <Camera/Camera2DController.h>
#include <Renderer/Mesh.h>
#include <Renderer/ParticleSystem.h>
#include <Renderer/Sprite.h>


//////////////////////////////////////////////////////////////////////////
// WorldLayer
class WorldLayer : public Plop::ApplicationLayer
{
public:
	virtual void OnRegistered() override;
	virtual void OnUnregistered() override;
	virtual void OnUpdate(Plop::TimeStep& _timeStep) override;

	virtual uint8_t GetPriority() const override { return 20; }

private:
	Plop::Camera2DController	m_CameraController;
	Plop::TexturePtr			m_xSpritesheet;
	Plop::Sprite				m_grass;
	Plop::Sprite				m_dirt;
	Plop::Sprite				m_water;
};

