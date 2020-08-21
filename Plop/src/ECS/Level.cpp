#include "Plop_pch.h"
#include "Level.h"

#include <Renderer/Renderer.h>
#include <ECS/Entity.h>
#include <ECS/BaseComponents.h>

namespace Plop
{
	LevelWeakPtr Level::s_xCurrentLevel;

	Level::~Level()
	{
	}

	void Level::Init()
	{
		s_xCurrentLevel = weak_from_this();
	}

	void Level::Shutdown()
	{
	}

	void Level::Update( TimeStep _ts )
	{
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();
		auto& view = m_ENTTRegistry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto&[camera, transform] = view.get<CameraComponent, TransformComponent>( entity );
			xCurrentCamera = camera.xCamera;
			mViewMatrix = glm::inverse( transform.mTransform );
		}

		if (xCurrentCamera)
		{
			Renderer2D::PrepareScene( xCurrentCamera->GetProjectionMatrix(), mViewMatrix );

			auto& group = m_ENTTRegistry.group<TransformComponent, SpriteRendererComponent>();
			for (auto entity : group)
			{
				SpriteRendererComponent& renderer = group.get<SpriteRendererComponent>( entity );
				if (!renderer.xSprite)
					continue;

				TransformComponent& transform = group.get<TransformComponent>( entity );

				Renderer2D::DrawSprite( *renderer.xSprite, transform.mTransform );
			}

			Renderer2D::EndScene();
		}
	}

	Entity Level::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, weak_from_this() };

		e.AddComponent<NameComponent>( _sName );
		e.AddComponent<TransformComponent>();

		return e;
	}
}
