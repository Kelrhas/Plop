#include "Plop_pch.h"
#include "LevelBase.h"

#include <fstream>
#include <filesystem>
#include <tuple>

#include <entt/meta/resolve.hpp>

#include "Application.h"
#include "Renderer/Renderer.h"
#include "ECS/Components/Component_ParticleSystem.h"
#include "ECS/Entity.h"
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/ComponentIncludes.h"

#include "Events/EventDispatcher.h"
#include "Events/EntityEvent.h"

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

namespace Plop
{
	LevelBase::~LevelBase()
	{
	}

	void LevelBase::Init()
	{
		m_ENTTRegistry.sort<Component_GraphNode, Component_Transform>(); // minimizes cache misses when iterating together


#define MACRO_COMPONENT(comp)	BindOnCreate<Component_##comp>( m_ENTTRegistry ); \
								BindOnDestroy<Component_##comp>( m_ENTTRegistry );
	#include "ECS/Components/ComponentList.h"
#undef MACRO_COMPONENT
	}

	void LevelBase::Shutdown()
	{
		m_ENTTRegistry.clear();
	}

	void LevelBase::StartFromEditor()
	{
		Init();
	}

	void LevelBase::StopToEditor()
	{
		Shutdown();
	}

	void LevelBase::UpdateInEditor( TimeStep _ts )
	{
		DrawSprites();
	}

	bool LevelBase::BeforeUpdate()
	{
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();

		auto& view = m_ENTTRegistry.view<Component_Camera, Component_Transform>();
		for (auto entity : view)
		{
			auto& [camera, transform] = view.get<Component_Camera, Component_Transform>( entity );
			xCurrentCamera = camera.xCamera;
			mViewMatrix = glm::inverse( transform.GetWorldMatrix() );
		}

		m_xCurrentCamera = xCurrentCamera;
		m_mCurrentCameraView = mViewMatrix;
		if(xCurrentCamera)
			m_xCurrentCamera.lock()->SetViewMatrix( mViewMatrix );

		return xCurrentCamera != nullptr;
	}

	void LevelBase::Update( TimeStep& _ts )
	{
		DrawSprites();
		DrawParticles( _ts );
	}

	void LevelBase::AfterUpdate()
	{

	}

	Entity LevelBase::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		entt::entity entityID = m_ENTTRegistry.create();
		Entity e = { entityID, Application::GetCurrentLevel()};

		e.AddComponent<Component_Name>( _sName );
		e.AddComponent<Component_GraphNode>();
		e.AddComponent<Component_Transform>();

		EventDispatcher::SendEvent( EntityCreatedEvent( e ) );

		return e;
	}

	Entity LevelBase::CreateEntityWithHint( entt::entity _id )
	{
		entt::entity entityID = m_ENTTRegistry.create( _id );
		Entity e = { entityID, Application::GetCurrentLevel()};

		e.AddComponent<Component_Name>();
		e.AddComponent<Component_GraphNode>();
		e.AddComponent<Component_Transform>();

		EventDispatcher::SendEvent( EntityCreatedEvent( e ) );

		return e;
	}

	Entity LevelBase::GetEntityFromHint( entt::entity _id )
	{
		Entity e = {_id, Application::GetCurrentLevel() };

		return e;
	}

	void LevelBase::DestroyEntity( Entity& _entity )
	{
		EventDispatcher::SendEvent( EntityDestroyedEvent( _entity ) );

		_entity.SetParent( Entity() );

		static std::vector<Entity> vecChildren;
		_entity.GetChildren(vecChildren);
		for (Entity& e : vecChildren)
		{
			DestroyEntity( std::move(e) );
		}
		m_ENTTRegistry.destroy( _entity );
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

			size_t fileSize = levelFile.tellg();
			levelFile.seekg( 0, std::ios::end );
			fileSize = (size_t)levelFile.tellg() - fileSize;
			if (fileSize > 10)
			{
				levelFile.seekg( std::ios::beg );
				json jLevel;

				levelFile >> jLevel;

				FromJson( jLevel );
			}
			return true;
		}

		return false;
	}

	void LevelBase::CopyFrom( LevelBasePtr _xLevel )
	{
		auto& srcReg = _xLevel->m_ENTTRegistry;
		auto& destReg = m_ENTTRegistry;

		destReg.clear();
		destReg.assign( srcReg.data(), srcReg.data() + srcReg.size() );

		srcReg.visit( [&srcReg, &destReg]( auto _comp )
			{
				auto& type = entt::resolve_type( _comp );
				auto& f = type.func( "clone"_hs );
				f.invoke( {}, std::ref( srcReg ), std::ref( destReg ) );
			} );
	}

	json LevelBase::ToJson()
	{
		json j;

		m_ENTTRegistry.each( [&j, this]( entt::entity _entityID ) {
			Entity entity{ _entityID, Application::GetCurrentLevel() };
			if (!entity.HasFlag( EntityFlag::DYNAMIC_GENERATION ))
			{
				String& sName = entity.GetComponent<Component_Name>().sName;
				j["entities"].push_back( entity.ToJson() );
			}
		});

		return j;
	}

	void LevelBase::FromJson( const json& _j )
	{
		if (_j.contains( "entities" ))
		{
			// create all entities so that everything is created to apply GraphNode links
			for (auto j : _j["entities"])
			{
				CreateEntityWithHint( j["HintID"] );
			}

			// and apply whatever we need to
			for (auto j : _j["entities"])
			{
				Entity e = GetEntityFromHint(j["HintID"]);
				e.FromJson(j);
			}
		}
	}

	void LevelBase::DrawSprites()
	{
		using tuple_t = std::tuple<SpritePtr, glm::mat4, entt::id_type>;
		std::vector<tuple_t> vecSpriteMat;
		auto& group = m_ENTTRegistry.group<Component_Transform, Component_SpriteRenderer>();
		for (auto entityID : group)
		{
			Component_SpriteRenderer& renderer = group.get<Component_SpriteRenderer>( entityID );
			if (!renderer.xSprite)
				continue;


			Component_Transform& transform = group.get<Component_Transform>( entityID );

			glm::mat4 mTransform = transform.GetWorldMatrix();

			vecSpriteMat.push_back( std::make_tuple( renderer.xSprite, mTransform, entt::id_type(entityID)) );
		}

		// depth sorting
		std::sort(vecSpriteMat.begin(), vecSpriteMat.end(), [](const tuple_t &_pair1, const tuple_t &_pair2){
			return std::get<1>(_pair1)[3][2] < std::get<1>(_pair2)[3][2];
		});

		for (const tuple_t &_pair : vecSpriteMat)
		{
			Renderer::PushEntityId(std::get<2>(_pair));
			Renderer::DrawSprite( *std::get<0>(_pair), std::get<1>(_pair));
			Renderer::PopEntityId();
		}
	}

	void LevelBase::DrawParticles( const TimeStep& _ts )
	{
		auto& view = m_ENTTRegistry.view<Component_ParticleSystem>();
		for (auto [entityID, particleSystem] : view.proxy())
		{
			particleSystem.Update( _ts );
		}
	}
}
