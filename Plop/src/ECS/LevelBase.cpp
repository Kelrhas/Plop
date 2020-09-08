#include "Plop_pch.h"
#include "LevelBase.h"

#include <Renderer/Renderer.h>
#include <ECS/Entity.h>
#include <ECS/BaseComponents.h>

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

namespace Plop
{
	LevelBaseWeakPtr LevelBase::s_xCurrentLevel;

	LevelBase::~LevelBase()
	{
	}

	void LevelBase::Init()
	{
		s_xCurrentLevel = weak_from_this();
	}

	void LevelBase::Shutdown()
	{
	}

	bool LevelBase::BeforeUpdate()
	{
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();
		auto& view = m_ENTTRegistry.view<CameraComponent, TransformComponent>();
		for (auto entity : view)
		{
			auto& [camera, transform] = view.get<CameraComponent, TransformComponent>( entity );
			xCurrentCamera = camera.xCamera;
			mViewMatrix = glm::inverse( transform.mTransform );
		}

		if (xCurrentCamera)
		{
			Renderer2D::PrepareScene( xCurrentCamera->GetProjectionMatrix(), mViewMatrix );
			m_bRendering = true;
		}

		return m_bRendering;
	}

	void LevelBase::Update( TimeStep _ts )
	{
		auto& group = m_ENTTRegistry.group<TransformComponent, SpriteRendererComponent>();
		for (auto entityID : group)
		{
			SpriteRendererComponent& renderer = group.get<SpriteRendererComponent>( entityID );
			if (!renderer.xSprite)
				continue;

			TransformComponent& transform = group.get<TransformComponent>( entityID );
			
			Entity entity{ entityID, GetCurrentLevel() };
			glm::mat4 mTransform = transform.mTransform;
			while (entity = entity.GetParent())
			{
				mTransform *= entity.GetComponent<TransformComponent>().mTransform;
			}


			Renderer2D::DrawSprite( *renderer.xSprite, mTransform );
		}
	}

	void LevelBase::AfterUpdate()
	{
		if (m_bRendering)
			Renderer2D::EndScene();
		m_bRendering = false;
	}

	Entity LevelBase::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, weak_from_this() };

		e.AddComponent<NameComponent>( _sName );
		e.AddComponent<GraphNodeComponent>();
		e.AddComponent<TransformComponent>();

		return e;
	}
}
