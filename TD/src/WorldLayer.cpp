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

	if (m_xSpritesheet == nullptr)
	{
		m_xSpritesheet = Plop::Texture::Create2D( "TD/assets/textures/tilesheet.png" );

		m_grass.SetTexture( m_xSpritesheet );
		m_grass.SetSpriteIndex( { 1, 11 }, { 23, 13 } );
		m_dirt.SetTexture( m_xSpritesheet );
		m_dirt.SetSpriteIndex( { 1, 8 }, { 23, 13 } );
		m_water.SetTexture( m_xSpritesheet );
		m_water.SetSpriteIndex( { 11, 11 }, { 23, 13 } );
	}
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

	Plop::Renderer2D::PrepareScene(m_CameraController.GetCamera());

	{
		PROFILING_SCOPE( "Drawing world" );
		static int nb = 5;
		ImGui::DragInt( "Grid", &nb, 0.1f, 0, 50 );

		for (int i = -nb; i < nb; ++i)
		{
			for (int j = -nb; j < nb; ++j)
			{
				Plop::Renderer2D::DrawSprite( { i, j }, { 1.f, 1.f }, (i*j % 3 == 0) ? m_grass : (i*j % 3 == 1) ? m_dirt : m_water );
			}
		}
	}

	Plop::Renderer2D::EndScene();
}
