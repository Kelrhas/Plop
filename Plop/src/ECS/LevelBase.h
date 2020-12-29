#pragma once

#include <json.hpp>
using json = nlohmann::json;

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

#include <TimeStep.h>

namespace Plop
{
	class Entity;

	class LevelBase;
	using LevelBasePtr = std::shared_ptr<LevelBase>;
	using LevelBaseWeakPtr = std::weak_ptr<LevelBase>;

	/**
	 * Hold entities
	 */
	class LevelBase : public std::enable_shared_from_this<LevelBase>
	{
		friend class Entity;
		friend class EditorLayer;
	public:
		LevelBase() = default;
		virtual ~LevelBase();

		virtual void Init();
		virtual void Shutdown();

		virtual void StartFromEditor();
		virtual void StopToEditor();
		virtual	void UpdateInEditor( TimeStep _ts );

				bool BeforeUpdate();			// prepare the rendering
		virtual	void Update( TimeStep _ts );	// update & submit rendering
				void AfterUpdate();				// close the rendering

				Entity CreateEntity( const String& _sName = "New Entity" );
				Entity CreateEntityWithHint( entt::entity _id );
				void DestroyEntity( Entity& _entity );


				entt::registry& GetEntityRegistry() { return m_ENTTRegistry; }
				const entt::registry& GetEntityRegistry() const { return m_ENTTRegistry; }


				void MakeCurrent();
				void Save( const StringPath& _path );
				bool Load( const StringPath& _path );
		virtual json ToJson();
		virtual void FromJson(const json& _j);


		static LevelBaseWeakPtr GetCurrentLevel() { return s_xCurrentLevel; }


	protected:
		static LevelBaseWeakPtr s_xCurrentLevel;
		entt::registry	m_ENTTRegistry;

	private:

				void	DrawSprites();

		bool			m_bRendering = false;

	};
}
