#include "Plop_pch.h"
#include "LevelBase.h"

#include <fstream>
#include <filesystem>

#include <Application.h>
#include <Renderer/Renderer.h>
#include <ECS/Entity.h>
#include <ECS/BaseComponents.h>

#include <Events/EventDispatcher.h>
#include <Events/EntityEvent.h>

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

namespace Plop
{
	LevelBaseWeakPtr LevelBase::s_xCurrentLevel;

	LevelBase::~LevelBase()
	{
	}

	void LevelBase::Init()
	{
	}

	void LevelBase::Shutdown()
	{
		m_ENTTRegistry.clear();
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
			mViewMatrix = glm::inverse( transform.GetMatrix() );
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
			
			Entity entity{ entityID, weak_from_this() };
			glm::mat4 mTransform = transform.GetMatrix();
			while (entity = entity.GetParent())
			{
				mTransform *= entity.GetComponent<TransformComponent>().GetMatrix();
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

	void LevelBase::DestroyEntity( Entity& _entity )
	{
		EventDispatcher::SendEvent( EntityDestroyedEvent( _entity ) );

		_entity.SetParent( Entity() );

		std::vector<Entity> vecChildren = _entity.GetChildren();
		for (Entity& e : vecChildren)
		{
			DestroyEntity( std::move(e) );
		}
		m_ENTTRegistry.destroy( _entity );
	}

	void LevelBase::MakeCurrent()
	{
		s_xCurrentLevel = weak_from_this();
	}

	void LevelBase::Save()
	{
		// TODO: open Save file browser if not yet saved
		std::filesystem::path filePath( "Sample/data/levels/test.level" );
		std::filesystem::create_directories( filePath.parent_path() );
		std::ofstream levelFile( filePath, std::ios::out | std::ios::trunc );
		if (levelFile.is_open())
		{
			json jLevel = this->ToJson();

			levelFile << jLevel.dump( 2 );
		}
	}

	bool LevelBase::Load()
	{
		String sLevel = "Sample/data/levels/test.level";
		std::ifstream levelFile( sLevel.c_str(), std::ios::in );
		if (levelFile.is_open())
		{
			Application::GetConfig().sLastLevelActive = sLevel;

			Shutdown();
			Init();

			json jLevel = this->ToJson();

			levelFile >> jLevel;

			FromJson( jLevel );

			return true;
		}

		return false;
	}

	json LevelBase::ToJson()
	{
		json j;

		m_ENTTRegistry.each( [&j, this]( entt::entity _entityID ) {
			Entity entity{ _entityID, weak_from_this() };
			if (!entity.GetParent())
			{
				String& sName = entity.GetComponent<NameComponent>().sName;
				j["entities"].push_back( entity.ToJson() );
			}
		});

		return j;
	}

	void LevelBase::FromJson( const json& _j )
	{
		if (_j.contains( "entities" ))
		{
			for (auto j : _j["entities"])
			{
				Entity e = CreateEntity();
				e.FromJson( j );
			}
		}
	}
}
