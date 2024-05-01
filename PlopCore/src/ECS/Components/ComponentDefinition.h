#pragma once

#include <type_traits>

#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>

namespace Plop
{

	//////////////////////////////////////////////////////////////////////////
	// EditorUI
	//////////////////////////////////////////////////////////////////////////

	template<typename Comp>
	concept EditorUICallable = requires (Comp c) { c.EditorUI(); };

	template<class Comp>
	constexpr static void AmbiguousCallComponentEditorUI(entt::registry &, entt::entity) {}

	template<class Comp> requires EditorUICallable<Comp>
	constexpr static void AmbiguousCallComponentEditorUI( entt::registry& registry, entt::entity _entity )
	{
		ASSERTM(registry.all_of<Comp>(_entity), "The entity does not have this component");
		Comp& comp = registry.get<Comp>( _entity );
		comp.EditorUI();
	}
	
	template<typename Comp>
	constexpr static void CallComponentEditorUI(entt::registry& _registry, entt::entity _entity)
	{
		AmbiguousCallComponentEditorUI<Comp>(_registry, _entity);
	}


	//////////////////////////////////////////////////////////////////////////
	// AfterLoad
	//////////////////////////////////////////////////////////////////////////
	
	template<typename Comp>
	concept AfterLoadCallable = requires (Comp c) { c.AfterLoad(); };
							
	template<typename Comp>
	constexpr static void AmbiguousCallComponentAfterLoad( entt::registry&, entt::entity) {}

	template<typename Comp> requires AfterLoadCallable<Comp>
	constexpr static void AmbiguousCallComponentAfterLoad( entt::registry& _registry, entt::entity _entity)
	{
		ASSERTM(_registry.all_of<Comp>(_entity), "The entity does not have this component");
		Comp &comp = _registry.get<Comp>(_entity);
		comp.AfterLoad();
	}
	
	template<typename Comp>
	constexpr static void CallComponentAfterLoad(entt::registry& _registry, entt::entity _entity)
	{
		AmbiguousCallComponentAfterLoad<Comp>(_registry, _entity);
	}


	//////////////////////////////////////////////////////////////////////////
	// FromJson
	//////////////////////////////////////////////////////////////////////////
	
	template<typename Comp, typename JsonType>
	concept FromJsonCallable = requires (Comp c, const JsonType &j) { c.FromJson(j); };
							
	template<typename Comp, typename JsonType>
	constexpr static void AmbiguousCallComponentFromJson( entt::registry&, entt::entity, const JsonType&) {}

	template<typename Comp, typename JsonType> requires FromJsonCallable<Comp, JsonType>
	constexpr static void AmbiguousCallComponentFromJson( entt::registry& _registry, entt::entity _entity, const JsonType& _json )
	{
		Comp& comp = _registry.get_or_emplace<Comp>( _entity );
		comp.FromJson(_json);
	}
	
	template<typename Comp, typename JsonType>
	constexpr static void CallComponentFromJson(entt::registry& _registry, entt::entity _entity, const JsonType& _json)
	{
		AmbiguousCallComponentFromJson<Comp>(_registry, _entity, _json);
	}


	//////////////////////////////////////////////////////////////////////////
	// ToJson
	//////////////////////////////////////////////////////////////////////////
	
	template<typename Comp, typename JsonType>
	concept ToJsonCallable = requires (Comp c) { { c.ToJson() } -> std::same_as<JsonType>; };

	template<typename Comp, typename JsonType>
	constexpr static JsonType AmbiguousCallComponentToJson(const entt::registry&, entt::entity) { return JsonType{}; }

	template<typename Comp, typename JsonType> requires ToJsonCallable<Comp, JsonType>
	constexpr static JsonType AmbiguousCallComponentToJson(const entt::registry& _registry, entt::entity _entity)
	{
		ASSERTM(_registry.all_of<Comp>(_entity), "The entity does not have this component");
		const Comp& comp = _registry.get<Comp>( _entity );
		return comp.ToJson();
	}
	
	template<typename Comp, typename JsonType>
	constexpr static JsonType CallComponentToJson(const entt::registry& _registry, entt::entity _entity)
	{
		return AmbiguousCallComponentToJson<Comp, JsonType>(_registry, _entity);
	}


	//////////////////////////////////////////////////////////////////////////
	// CloneRegistry
	//////////////////////////////////////////////////////////////////////////

	template<class Comp, class RegistryType>
	constexpr static void CloneAllRegistryComponents(const RegistryType &_src, RegistryType &_dst)
	{
		//auto view = _src.view<Comp>();
		//if constexpr (ENTT_IS_EMPTY( Comp )::value)
		//	_dst.insert<Comp>( view.data(), view.data() + view.size() );
		//else
		//	_dst.insert<Comp>( view.data(), view.data() + view.size(), view.raw(), view.raw() + view.size() );

		// @nocheckin test
		//auto &srcStorage = _src.storage<Comp>();
		//_dst.storage<Comp>().insert(srcStorage.data(), srcStorage.data() + srcStorage.size(), srcStorage.raw(), srcStorage.raw() + srcStorage.size());

		assert(false);
	}

	//////////////////////////////////////////////////////////////////////////
	// DuplicateComponent
	//////////////////////////////////////////////////////////////////////////

	template<class Comp, class RegistryType, class EntityType>
	void CallDuplicateComponent(const RegistryType &_regSrc, EntityType _entitySrc, RegistryType &_regDst, EntityType _entityDst)
	{
		// @check
		if (_regSrc.all_of<Comp>(_entitySrc))
		{
			Comp &compDst = _regDst.template get_or_emplace<Comp>(_entityDst);
			const Comp &compSrc = _regSrc.template get<Comp>(_entitySrc);
			compDst = compSrc;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// OnCreate
	//////////////////////////////////////////////////////////////////////////

	template <class, class = void>
	struct HasOnCreate : std::false_type {};
	template <class Comp>
	struct HasOnCreate<Comp, std::void_t<decltype(&Comp::OnCreate)>>
	  : std::is_same<void, decltype(std::declval<Comp>().OnCreate(std::declval<entt::registry &>(), std::declval<entt::entity>()))>
	{
	};

	template<class Comp, class RegistryType, std::enable_if_t<!HasOnCreate<Comp>::value, bool> = true>
	constexpr static void BindOnCreate( RegistryType& ) {}


	template<class Comp, class RegistryType, std::enable_if_t<HasOnCreate<Comp>::value, bool> = true>
	constexpr static void BindOnCreate( RegistryType& registry )
	{
		// @nocheckin test
		registry.on_construct<Comp>().connect<entt::invoke<&Comp::OnCreate>>();
	}



	//////////////////////////////////////////////////////////////////////////
	// OnDestroy
	//////////////////////////////////////////////////////////////////////////


	template <class, class = void>
	struct HasOnDestroy : std::false_type {};
	template <class Comp>
	struct HasOnDestroy<Comp, std::void_t<decltype(&Comp::OnDestroy)>>
	  : std::is_same<void, decltype(std::declval<Comp>().OnDestroy(std::declval<entt::registry &>(), std::declval<entt::entity>()))>
	{
	};

	template<class Comp, class RegistryType, std::enable_if_t<!HasOnDestroy<Comp>::value, bool> = true>
	constexpr static void BindOnDestroy( RegistryType& ) {}

	template<class Comp, class RegistryType, std::enable_if_t<HasOnDestroy<Comp>::value, bool> = true>
	constexpr static void BindOnDestroy( RegistryType& registry )
	{
		// @nocheckin test
		registry.on_destroy<Comp>().connect<entt::invoke<&Comp::OnDestroy>>();
	}




	//////////////////////////////////////////////////////////////////////////
	// HasComponent
	//////////////////////////////////////////////////////////////////////////
	template<class Comp, class RegistryType, class EntityType>
	constexpr bool HasComponent(RegistryType &registry, EntityType _entity)
	{
		return registry.template all_of<Comp>(_entity);
	}
	
	//////////////////////////////////////////////////////////////////////////
	// CanAddComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp>
	constexpr bool CanAddComponent() { return true; }


	//////////////////////////////////////////////////////////////////////////
	// CanRemoveComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp>
	constexpr bool CanRemoveComponent() { return true; }



	//////////////////////////////////////////////////////////////////////////
	// CanEditComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp>
	constexpr bool CanEditComponent()
	{
		return EditorUICallable<Comp>;
	}

	//////////////////////////////////////////////////////////////////////////
	// AddComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp, class RegistryType, class EntityType>
	constexpr void AddComponent( RegistryType& registry, EntityType _entity )
	{
		if (!registry.template all_of<Comp>(_entity))
			registry.template emplace<Comp>( _entity );
	}

	//////////////////////////////////////////////////////////////////////////
	// RemoveComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp, class RegistryType, class EntityType>
	constexpr void RemoveComponent( RegistryType& registry, EntityType _entity )
	{
		registry.template remove<Comp>( _entity );
	}


	//template<class Comp, class RegistryType, class EntityType>
	//bool AddComponent( const RegistryType& registry, EntityType _entity )
	//{
	//	constexpr auto type[] = { entt::type_info<Comp>::id() };
	//	return registry.runtime_view( std::cbegin( type ), std::cend( type ) ).contains( _entity );
	//}


	//template<class Comp, class RegistryType, class EntityType>
	//bool HasComponent( const RegistryType& registry, EntityType _entity )
	//{
	//	constexpr auto type[] = { entt::type_info<Comp>::id() };
	//	return registry.runtime_view( std::cbegin( type ), std::cend( type ) ).contains( _entity );
	//}

}