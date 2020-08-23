#include "TD_pch.h"
#include "WorldLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Debug/Debug.h>
#include <Constants.h>
#include <ECS/BaseComponents.h>
#include <Events/EventDispatcher.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>
#include <Renderer/ParticleSystem.h>
#include <Renderer/Sprite.h>

void WorldLayer::OnRegistered()
{
	m_xLevel = std::make_shared<Level>();
	m_xLevel->Init();

	Plop::Entity cameraEntity = m_xLevel->CreateEntity( "Camera" );
	cameraEntity.GetComponent<Plop::TransformComponent>().SetPosition( glm::vec3( 3.7, 1.2, 0. ) );
	Plop::CameraComponent& cameraComp = cameraEntity.AddComponent<Plop::CameraComponent>();
	cameraComp.xCamera = std::make_shared<Plop::Camera>();
	cameraComp.xCamera->Init();
}

void WorldLayer::OnUnregistered()
{
	m_xLevel = nullptr;
}

void WorldLayer::OnUpdate(Plop::TimeStep& _ts)
{
	PROFILING_FUNCTION();

	Plop::Renderer::Clear();

	if (m_xLevel->BeforeUpdate())
	{
		m_xLevel->Update( _ts );
		m_xLevel->AfterUpdate();
	}
}
