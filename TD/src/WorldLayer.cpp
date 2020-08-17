#include "TD_pch.h"
#include "WorldLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <Debug/Debug.h>
#include <Constants.h>
#include <Events/EventDispatcher.h>
#include <Input/Input.h>
#include <Renderer/Renderer.h>
#include <Renderer/Texture.h>

void WorldLayer::OnRegistered()
{
	Plop::EventDispatcher::RegisterListener(&m_CameraController);

	m_CameraController.Init();
	m_Level.Init();
}

void WorldLayer::OnUnregistered()
{
	Plop::EventDispatcher::UnregisterListener(&m_CameraController);
}

void WorldLayer::OnUpdate(Plop::TimeStep& _timeStep)
{
	PROFILING_FUNCTION();

	m_CameraController.OnUpdate(_timeStep);

	Plop::Renderer::Clear();

	Plop::Renderer2D::PrepareScene( m_CameraController.GetCamera() );

	m_Level.Draw();

	Plop::Renderer2D::EndScene();
}
