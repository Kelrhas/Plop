#include "Config.h"

#include "LevelBase.h"

#include "Application.h"
#include "ECS/Components/BaseComponents.h"
#include "ECS/Components/ComponentDefinition.h"
#include "ECS/Components/ComponentIncludes.h"
#include "ECS/Components/Component_ParticleSystem.h"
#include "ECS/Entity.h"
#include "ECS/PrefabManager.h"
#include "ECS/Serialisation.h"
#include "Events/EntityEvent.h"
#include "Events/EventDispatcher.h"
#include "Renderer/Renderer.h"
#include "Utils/JsonTypes.h"

#include <entt/meta/resolve.hpp>
#include <filesystem>
#include <fstream>
#include <tuple>

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?

namespace Plop
{
	LevelBase::~LevelBase()
	{
	}

	void LevelBase::Init()
	{
		EventDispatcher::RegisterListener(this);

		// minimizes cache misses when iterating together
		// CAREFUL sorting is incompatible with group and will assert
		//m_ENTTRegistry.sort 

		m_ENTTRegistry.ctx().emplace<Plop::GUID>() = GUID();
		m_ENTTRegistry.ctx().emplace<LevelBase*>() = this;


#define MACRO_COMPONENT(comp)	BindOnCreate<Component_##comp>( m_ENTTRegistry ); \
								BindOnDestroy<Component_##comp>( m_ENTTRegistry );
	#include "ECS/Components/ComponentList.h"
#undef MACRO_COMPONENT
	}

	void LevelBase::Shutdown()
	{
		// @check
		m_ENTTRegistry.clear();

		EventDispatcher::UnregisterListener(this);
	}

	bool LevelBase::OnEvent(Event *_pEvent)
	{
		if (_pEvent->GetEventType() == EventType::PrefabInstantiatedEvent)
		{
			PrefabInstantiatedEvent &entityEvent = *(PrefabInstantiatedEvent *)_pEvent;

			GUID *pGUIDLevel = m_ENTTRegistry.ctx().find<GUID>();
			ASSERTM(pGUIDLevel, "No GUID for this level");
			if (!pGUIDLevel)
				return false;

			if (entityEvent.entity.IsFromLevel(*pGUIDLevel))
			{
				std::stack<entt::entity> todo;
				todo.push(entityEvent.entity);


				while (!todo.empty())
				{
					entt::entity enttID = todo.top();
					todo.pop();

					auto &nameComp = m_ENTTRegistry.get<Component_Name>(enttID);
					ASSERTM([&]() {
						  auto it = m_mapGUIDToEntt.find(nameComp.guid);
						  return it == m_mapGUIDToEntt.end() || it->second == enttID;
					  }(), "There already is a mapping with this guid {}", nameComp.guid);

					m_mapGUIDToEntt[nameComp.guid] = enttID;

					auto &graphComp = m_ENTTRegistry.get<Component_GraphNode>(enttID);
					if (graphComp.firstChild != entt::null)
						todo.push(graphComp.firstChild);
					if (graphComp.nextSibling != entt::null)
						todo.push(graphComp.nextSibling);
				}
			}
		}

		return false;
	}

	void LevelBase::StartFromEditor()
	{
		OnStart();
	}

	void LevelBase::StopToEditor()
	{
		OnStop();

		m_mapGUIDToEntt.clear();
		m_ENTTRegistry.clear();
	}

	void LevelBase::UpdateInEditor( const TimeStep &_ts )
	{
		DrawSprites();
		if (!Application::Get()->GetEditor().IsEditing())
			DrawParticles(_ts);
	}

	void LevelBase::OnStart() {}

	bool LevelBase::BeforeUpdate()
	{
		CameraPtr xCurrentCamera = nullptr;
		glm::mat4 mViewMatrix = glm::identity<glm::mat4>();

		auto view = m_ENTTRegistry.view<Component_Camera, Component_Transform>();
		for (auto entity : view)
		{
			auto [camera, transform] = view.get<Component_Camera, Component_Transform>(entity);
			xCurrentCamera			 = camera.GetCamera();
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
		PROFILING_FUNCTION();

		DrawSprites();
		DrawParticles( _ts );
	}

	void LevelBase::AfterUpdate()
	{

	}

	void LevelBase::OnStop() {}

	Entity LevelBase::CreateEntity( const String& _sName /*= "New Entity"*/ )
	{
		Entity e = { m_ENTTRegistry.create(), m_ENTTRegistry };

		auto &nameComp = e.AddComponent<Component_Name>( _sName );
		ASSERTM(m_mapGUIDToEntt.find(nameComp.guid) == m_mapGUIDToEntt.end(), "There already is a mapping with this guid {}", nameComp.guid);
		m_mapGUIDToEntt[nameComp.guid] = e;
		e.AddComponent<Component_GraphNode>();
		e.AddComponent<Component_Transform>();

		EventDispatcher::SendEvent(EntityCreatedEvent(e));

		return e;
	}

	Entity LevelBase::CreateEntityWithGUID(GUID _guid)
	{
		Entity e = { m_ENTTRegistry.create(), m_ENTTRegistry };

		e.AddComponent<Component_Name>(_guid);
		ASSERTM(m_mapGUIDToEntt.find(_guid) == m_mapGUIDToEntt.end(), "There already is a mapping with this guid {}", _guid);
		m_mapGUIDToEntt[_guid] = e;
		e.AddComponent<Component_GraphNode>();
		e.AddComponent<Component_Transform>();

		EventDispatcher::SendEvent( EntityCreatedEvent( e ) );

		return e;
	}

	Entity LevelBase::GetEntityFromGUID(GUID _guid)
	{
		auto it = m_mapGUIDToEntt.find(_guid);
		if (it != m_mapGUIDToEntt.end())
		{
			return Entity {it->second, m_ENTTRegistry};
		}

		ASSERT(false);
		return Entity();
	}

	void LevelBase::DestroyEntity( Entity _entity )
	{
		ASSERT(_entity);

		PROFILING_FUNCTION();
		_entity.SetParent( Entity() );

		std::vector<Entity> vecChildren;
		_entity.GetChildren(vecChildren);
		for (Entity& e : vecChildren)
		{
			DestroyEntity( e );
		}
		m_mapGUIDToEntt.erase(_entity.GetComponent<Component_Name>().guid);
		m_ENTTRegistry.destroy( _entity );

		EventDispatcher::SendEvent(EntityDestroyedEvent(_entity));
	}

	void LevelBase::Save( const StringPath& _path )
	{
		std::filesystem::create_directories(_path.parent_path() );
		std::ofstream levelFile(_path, std::ios::out | std::ios::trunc );
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
		std::ifstream levelFile(_path, std::ios::in );
		if (levelFile.is_open())
		{
			Application::GetConfig().sLastLevelActive = _path.string();
			Application::GetConfig().Save();

			Shutdown();
			Init();

			json jLevel;

			try
			{
				levelFile >> jLevel;
				FromJson( jLevel );
				
				return true;
			}
			catch (const json::parse_error &e)
			{
				Log::Error("JSON parsing error for {}:\n\t{}", _path.string().c_str(), e.what());
			}
			catch (const std::exception &e)
			{
				Log::Error("Unkown error when parsing {}:\n\t{}", _path.string().c_str(), e.what());
			}
		}

		return false;
	}

	void LevelBase::CopyFrom( LevelBasePtr _xLevel )
	{
		json  j		  = _xLevel->ToJson();

		std::ofstream levelFile("EditorLevel.lvl" , std::ios::out | std::ios::trunc);
		if (levelFile.is_open())
		{
			levelFile << j.dump(2);
		}

		FromJson(j);

		return;


		// not sure it we need to do more than juste serialise/deserialise
		// maybe for speed purpose
		// but good enough for now (and safe)


		auto& srcReg = _xLevel->m_ENTTRegistry;
		auto& destReg = m_ENTTRegistry;
		destReg.clear();


		// we may need to patch GraphNode while copying, because of some entities not copied (NO_SERIALISATION flag)
		// some entities may be the first child in the hierarchy but we still need to get retrieve the next sibling
		// to put it as first child of its parent ...
//#error TODO



		// first copy entities
		{
			auto &srcStorage = srcReg.storage<entt::entity>();
			auto &dstStorage = destReg.storage<entt::entity>();
			for (auto enttID : srcStorage)
			{
				if (Component_GraphNode *pCompNode = srcReg.try_get<Component_GraphNode>(enttID))
				{
					if (pCompNode->uFlags.Has(EntityFlag::NO_SERIALISATION))
						continue;
				}
				dstStorage.push(enttID);
			}
		}

		// then all components
		for (auto [id, srcStorage] : srcReg.storage())
		{
			auto *pDstStorage = destReg.storage(id);
			if (pDstStorage == nullptr)
			{
				entt::resolve(srcStorage.type()).invoke("storage"_hs, {}, entt::forward_as_meta(destReg), id);
				pDstStorage = destReg.storage(id);
			}
			ASSERT(pDstStorage);

			pDstStorage->reserve(srcStorage.size());
			for (auto it = srcStorage.begin(); it != srcStorage.end(); it++)
			{
				// push only if the entity is in the destination register
				if (destReg.valid(*it))
					pDstStorage->push(*it, srcStorage.value(*it));
			}
		}

		// notify loading finished
		// @nocheckin
		CHECK_THIS("Not sure if it is the correct moment to do that, but still need to (at least to generate the pathfind for now)");
		for (auto enttID : destReg.storage<entt::entity>())
		{
			Entity e { enttID, destReg };
//#error need to prevent creating/deleting entities while iterating through them ...
			/*	Component_HexGrid::AfterLoad calls HexGrid::Init that deletes and creates entities for all tiles
			* */
			e.AfterLoad();
		}
	}

	json LevelBase::ToJson()
	{
		json j;

		PrefabManager::VisitAllLibraries([&j](const String &_sName, const PrefabLibrary &_lib) {

			j[JSON_PREFABLIBS].push_back(_lib.sPath.string());
			return VisitorFlow::CONTINUE;
		});

		for (auto [_entityID] : m_ENTTRegistry.storage<entt::entity>().each())
		{
			Entity entity = { _entityID, m_ENTTRegistry };
			if (!entity.HasFlag(EntityFlag::NO_SERIALISATION))
			{
				String &sName = entity.GetComponent<Component_Name>().sName;
				if (entity.HasComponent<Component_PrefabInstance>() || !PrefabManager::IsPartOfPrefab(entity))
				{
					j[JSON_ENTITIES].push_back( entity.ToJson() );
				}
			}
		}

		return j;
	}

	void LevelBase::FromJson( const json& _j )
	{
		if (_j.contains(JSON_PREFABLIBS))
		{
			for (const String &s : _j[JSON_PREFABLIBS])
			{
				PrefabManager::LoadPrefabLibrary(s);
			}
		}

		if (_j.contains(JSON_ENTITIES))
		{
			// create all entities so that everything is created when to apply GraphNode links
			for (const auto &j : _j[JSON_ENTITIES])
			{
				CreateEntityWithGUID(j[JSON_GUID]);
			}

			// and apply whatever we need to
			for (const auto &j : _j[JSON_ENTITIES])
			{
				Entity e = GetEntityFromGUID(j[JSON_GUID]);
				e.FromJson(j);
			}

			// notify after every entities and components are created
			for (const auto &j : _j[JSON_ENTITIES])
			{
				Entity e = GetEntityFromGUID(j[JSON_GUID]);
				e.AfterLoad();
			}
		}
	}

	void LevelBase::DrawSprites()
	{
		PROFILING_FUNCTION();

		using tuple_t = std::tuple<SpritePtr, glm::mat4, entt::id_type>;
		std::vector<tuple_t> vecSpriteMat;
		auto group = m_ENTTRegistry.group<Component_GraphNode, Component_Transform, Component_SpriteRenderer>();
		for (auto entityID : group)
		{
			Component_GraphNode& graphComp = group.get<Component_GraphNode>(entityID);
			if (graphComp.uFlags.Has(EntityFlag::HIDE))
				continue;

			Component_SpriteRenderer& renderer = group.get<Component_SpriteRenderer>( entityID );
			if (!renderer.xSprite)
				continue;


			Component_Transform& transform = group.get<Component_Transform>( entityID );

			glm::mat4 mTransform = transform.GetWorldMatrix();

			vecSpriteMat.push_back( std::make_tuple( renderer.xSprite, mTransform, entt::id_type(entityID)) );
		}

		{
			PROFILING_SCOPE("Depth sorting");

			// depth sorting
			std::sort(vecSpriteMat.begin(), vecSpriteMat.end(), [](const tuple_t &_pair1, const tuple_t &_pair2) {
				return std::get<1>(_pair1)[3][2] < std::get<1>(_pair2)[3][2];
			});
		}

		for (const tuple_t &_pair : vecSpriteMat)
		{
			Renderer::PushEntityId(std::get<2>(_pair));
			Renderer::DrawSprite( *std::get<0>(_pair), std::get<1>(_pair));
			Renderer::PopEntityId();
		}
	}

	void LevelBase::DrawParticles( const TimeStep& _ts )
	{
		PROFILING_FUNCTION();

		auto view = m_ENTTRegistry.view<Component_ParticleSystem>();
		for (auto [entityID, particleSystem] : view.each())
		{
			particleSystem.Update( _ts );
		}
	}
}
