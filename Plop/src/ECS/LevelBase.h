#pragma once

#include <json.hpp>
using json = nlohmann::json;

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include "TimeStep.h"
#include "Camera/Camera.h"
#include "Events/IEventListener.h"
#include "Utils/GUID.h"

namespace Plop
{
	class Entity;

	class LevelBase;
	using LevelBasePtr = std::shared_ptr<LevelBase>;
	using LevelBaseWeakPtr = std::weak_ptr<LevelBase>;

	/**
	 * Hold entities
	 */
	class LevelBase : IEventListener
	{
		friend class Entity;
		friend class EditorLayer;
	public:
		LevelBase() = default;
		virtual ~LevelBase();

		virtual void Init();
		virtual void Shutdown();
		virtual bool OnEvent(Event &_event) override;

		virtual void StartFromEditor();
		virtual void StopToEditor();
		virtual	void UpdateInEditor( TimeStep _ts );

		virtual void OnStart();
				bool BeforeUpdate();			// prepare the rendering, returns if rendering can proceed
		virtual	void Update( TimeStep& _ts );	// update & submit rendering
				void AfterUpdate();				// close the rendering
		virtual void OnStop();

				Entity CreateEntity( const String& _sName = "New Entity" );
				Entity CreateEntityWithGUID(Plop::GUID _guid);
				Entity GetEntityFromGUID(Plop::GUID _guid);
				void DestroyEntity(Entity _entity);


				entt::registry& GetEntityRegistry() { return m_ENTTRegistry; }
				const entt::registry& GetEntityRegistry() const { return m_ENTTRegistry; }


				void Save( const StringPath& _path );
				bool Load( const StringPath& _path );
		virtual void CopyFrom( LevelBasePtr _xLevel );
		virtual json ToJson();
		virtual void FromJson(const json& _j);


				CameraWeakPtr GetCamera() const { return m_xCurrentCamera; }
				const glm::mat4& GetCameraViewMatrix() const { return m_mCurrentCameraView; }


	protected:
		entt::registry	m_ENTTRegistry;
		CameraWeakPtr	m_xCurrentCamera;
		glm::mat4		m_mCurrentCameraView;

	private:

				void	DrawSprites();
				void	DrawParticles( const TimeStep& _ts );



		std::unordered_map<GUID, entt::entity> m_mapGUIDToEntt;

	};
}
