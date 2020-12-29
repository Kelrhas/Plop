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

	void LevelBase::StartFromEditor()
	{

	}

	void LevelBase::StopToEditor()
	{

	}

	void LevelBase::UpdateInEditor( TimeStep _ts )
	{
		DrawSprites();
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
		DrawSprites();
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

	Entity LevelBase::CreateEntityWithHint( entt::entity _id )
	{
		entt::entity entityID = m_ENTTRegistry.create( _id );
		Entity e = { entityID, weak_from_this() };

		e.AddComponent<NameComponent>();
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

	void LevelBase::Save( const StringPath& _path )
	{
		StringPath filePath = _path.is_absolute() ? _path : Application::Get()->GetRootDirectory() / _path;
		std::filesystem::create_directories( filePath.parent_path() );
		std::ofstream levelFile( filePath, std::ios::out | std::ios::trunc );
		if (levelFile.is_open())
		{
			json jLevel = this->ToJson();

			levelFile << jLevel.dump( 2 );

			Application::GetConfig().sLastLevelActive = _path.string();
			Application::GetConfig().Save();
		}
	}

	bool LevelBase::Load( const StringPath& _path )
	{
		StringPath filePath = _path.is_absolute() ? _path : Application::Get()->GetRootDirectory() / _path;
		std::ifstream levelFile( filePath, std::ios::in );
		if (levelFile.is_open())
		{
			Application::GetConfig().sLastLevelActive = _path.string();
			Application::GetConfig().Save();

			Shutdown();
			Init();

			json jLevel;

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
				Entity e = CreateEntityWithHint( j["HintID"] );
				e.FromJson( j );
			}
		}
	}

	void LevelBase::DrawSprites()
	{
		std::vector<std::pair<SpritePtr, glm::mat4>> vecSpriteMat;
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

			vecSpriteMat.push_back( std::make_pair( renderer.xSprite, mTransform ) );
		}

		std::sort( vecSpriteMat.begin(), vecSpriteMat.end(), []( std::pair<SpritePtr, glm::mat4>& _pair1, std::pair<SpritePtr, glm::mat4>& _pair2 )
			{
				return _pair1.second[3][2] < _pair2.second[3][2];
			} );

		for (std::pair<SpritePtr, glm::mat4>& _pair : vecSpriteMat)
		{
			Renderer2D::DrawSprite( *_pair.first, _pair.second );
		}
	}
}
