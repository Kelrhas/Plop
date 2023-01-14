#pragma once

#include <type_traits>

#include <entt/entity/helper.hpp>

namespace Plop
{

#define MACRO_DETECTOR_DEFAULT(FuncName) \
	template <class, class = void>\
	struct Has##FuncName : std::false_type {};

#define MACRO_DETECTOR_DEFAULT_ARG1(FuncName) \
	template <class, class, class = void>\
	struct Has##FuncName : std::false_type {};

#define MACRO_DETECTOR_DEFAULT_RETURN MACRO_DETECTOR_DEFAULT_ARG1

#define MACRO_DETECTOR_OVERRIDE(FuncName)\
	template <class Comp>\
	struct Has##FuncName<Comp, std::void_t<decltype(&Comp::FuncName)>> : std::is_same<void, decltype(std::declval<Comp>().FuncName())> {};

#define MACRO_DETECTOR_OVERRIDE_ARG1(FuncName, ArgType1)\
	template <class Comp, class ArgType1>\
	struct Has##FuncName<Comp, ArgType1, std::void_t<decltype(&Comp::FuncName)>> : std::is_same<void, decltype(std::declval<Comp>().FuncName(std::declval<ArgType1>()))> {};


#define MACRO_DETECTOR_OVERRIDE_RETURN(FuncName, ReturnType)\
	template <class Comp, class ReturnType>\
	struct Has##FuncName<Comp, ReturnType, std::void_t<decltype(&Comp::FuncName)>> : std::is_same<ReturnType, decltype(std::declval<Comp>().FuncName())> {};



#define MACRO_AMBIGUOUS_DEFAULT(FuncName) \
	template<class Comp, class ... Args>\
	constexpr static void AmbiguousCallComponent##FuncName( Args&&... ) {}

	// TODO add a check for is_trivially_constructible to provide a base case
#define MACRO_AMBIGUOUS_DEFAULT_RETURN(FuncName, ReturnType) \
	template<class Comp, class ReturnType, class ... Args>\
	constexpr static ReturnType AmbiguousCallComponent##FuncName( Args&&... ) { static_assert(false, "Please provide your own fallback function"); }

#define MACRO_CALL_AMBIGUOUS(FuncName) \
	template<class Comp, class ... Args>\
	constexpr static void CallComponent##FuncName( Args&&...args )\
	{\
		AmbiguousCallComponent##FuncName<Comp>( std::forward<Args>( args )... );\
	}

#define MACRO_CALL_AMBIGUOUS_RETURN(FuncName, ReturnType) \
	template<class Comp, class ReturnType, class ... Args>\
	constexpr static ReturnType CallComponent##FuncName( Args&&...args )\
	{\
		return AmbiguousCallComponent##FuncName<Comp, ReturnType>( std::forward<Args>( args )... );\
	}


#define MACRO_ALL_DEFAULT(FuncName) \
	MACRO_DETECTOR_DEFAULT( FuncName ) \
	MACRO_DETECTOR_OVERRIDE( FuncName ) \
	MACRO_AMBIGUOUS_DEFAULT( FuncName )\
	MACRO_CALL_AMBIGUOUS( FuncName )

#define MACRO_ALL_DEFAULT_ARG1(FuncName, ArgType1) \
	MACRO_DETECTOR_DEFAULT_ARG1( FuncName ) \
	MACRO_DETECTOR_OVERRIDE_ARG1( FuncName, ArgType1 ) \
	MACRO_AMBIGUOUS_DEFAULT( FuncName )\
	MACRO_CALL_AMBIGUOUS( FuncName )


#define MACRO_ALL_DEFAULT_RETURN(FuncName, ReturnType) \
	MACRO_DETECTOR_DEFAULT_RETURN( FuncName ) \
	MACRO_DETECTOR_OVERRIDE_RETURN( FuncName, ReturnType ) \
	MACRO_AMBIGUOUS_DEFAULT_RETURN( FuncName, ReturnType )\
	MACRO_CALL_AMBIGUOUS_RETURN( FuncName, ReturnType )


	//////////////////////////////////////////////////////////////////////////
	// EditorUI
	//////////////////////////////////////////////////////////////////////////

	MACRO_ALL_DEFAULT( EditorUI )

	template<class Comp, class RegistryType, class EntityType, std::enable_if_t<HasEditorUI<Comp>::value, bool> = true>
	constexpr static void AmbiguousCallComponentEditorUI( RegistryType& registry, EntityType _entity )
	{
		ASSERTM( registry.has<Comp>( _entity ), "The entity does not have this component" );
		Comp& comp = registry.get<Comp>( _entity );
		comp.EditorUI();
	}


	//////////////////////////////////////////////////////////////////////////
	// FromJson
	//////////////////////////////////////////////////////////////////////////
	
	MACRO_ALL_DEFAULT_ARG1( FromJson, Json )

	template<class Comp, class RegistryType, class EntityType, class JsonType, std::enable_if_t<HasFromJson<Comp, JsonType>::value, bool> = true>
	constexpr static void AmbiguousCallComponentFromJson( RegistryType& registry, EntityType _entity, const JsonType& _json )
	{
		Comp& comp = registry.get_or_emplace<Comp>( _entity );
		comp.FromJson(_json);
	}


	//////////////////////////////////////////////////////////////////////////
	// FromJson
	//////////////////////////////////////////////////////////////////////////
	
	MACRO_ALL_DEFAULT(AfterLoad)

	template<class Comp, class RegistryType, class EntityType, std::enable_if_t<HasAfterLoad<Comp>::value, bool> = true>
	constexpr static void AmbiguousCallComponentAfterLoad(RegistryType &registry, EntityType _entity)
	{
		ASSERTM(registry.has<Comp>(_entity), "The entity does not have this component");
		Comp &comp = registry.get<Comp>(_entity);
		comp.AfterLoad();
	}



	//////////////////////////////////////////////////////////////////////////
	// ToJson
	//////////////////////////////////////////////////////////////////////////

	MACRO_DETECTOR_DEFAULT_RETURN( ToJson ) 
	MACRO_DETECTOR_OVERRIDE_RETURN( ToJson, Json )
	MACRO_CALL_AMBIGUOUS_RETURN( ToJson, Json )

	template<class Comp, class JsonType, class ... Args>
	constexpr static JsonType AmbiguousCallComponentToJson( Args&&... )
	{
		return JsonType{};
	}

	template<class Comp, class JsonType, class RegistryType, class EntityType, std::enable_if_t<HasToJson<Comp, JsonType>::value, bool> = true>
	constexpr static JsonType AmbiguousCallComponentToJson( RegistryType& registry, EntityType _entity)
	{
		ASSERTM( registry.has<Comp>( _entity ), "The entity does not have this component" );
		const Comp& comp = registry.get<Comp>( _entity );
		return comp.ToJson();
	}



	//////////////////////////////////////////////////////////////////////////
	// CloneRegistry
	//////////////////////////////////////////////////////////////////////////

	template<class Comp, class RegistryType>
	constexpr static void CloneAllRegistryComponents(const RegistryType &_src, RegistryType &_dst)
	{
		auto view = _src.view<Comp>();
		if constexpr (ENTT_IS_EMPTY( Comp )::value)
		{
			_dst.insert<Comp>( view.data(), view.data() + view.size() );
		}
		else
		{
			_dst.insert<Comp>( view.data(), view.data() + view.size(), view.raw(), view.raw() + view.size() );
		}
	}


	template<class Comp, class RegistryType, class EntityType>
	constexpr static void CloneRegistryComponent(const RegistryType &_regSrc, const EntityType &_entitySrc, RegistryType &_regDst, EntityType &_entityDst)
	{
		if (_regSrc.has<Comp>(_entitySrc))
		{
			const Comp &compSrc = _regSrc.template get<Comp>(_entitySrc);
			Comp &compDst = _regDst.template get_or_emplace<Comp>(_entityDst);
			compDst = compSrc;
		}
	}



	//////////////////////////////////////////////////////////////////////////
	// DuplicateComponent
	//////////////////////////////////////////////////////////////////////////

	template<class Comp, class RegistryType, class EntityType>
	void CallDuplicateComponent(const RegistryType &_regSrc, EntityType _entitySrc, RegistryType &_regDst, EntityType _entityDst)
	{
		// @check
		if (_regSrc.has<Comp>(_entitySrc))
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
	struct HasOnCreate<Comp, std::void_t<decltype(&Comp::OnCreate)>> : std::is_same<void, decltype(std::declval<Comp>().OnCreate())> {};

	template<class Comp, class RegistryType, std::enable_if_t<!HasOnCreate<Comp>::value, bool> = true>
	constexpr static void BindOnCreate( RegistryType& ) {}

	template<class Comp, class RegistryType, std::enable_if_t<HasOnCreate<Comp>::value, bool> = true>
	constexpr static void BindOnCreate( RegistryType& registry )
	{
		registry.on_construct<Comp>().connect<&entt::invoke<&Comp::OnCreate>>();
	}



	//////////////////////////////////////////////////////////////////////////
	// OnDestroy
	//////////////////////////////////////////////////////////////////////////


	template <class, class = void>
	struct HasOnDestroy : std::false_type {};
	template <class Comp>
	struct HasOnDestroy<Comp, std::void_t<decltype(&Comp::OnDestroy)>> : std::is_same<void, decltype(std::declval<Comp>().OnDestroy())> {};

	template<class Comp, class RegistryType, std::enable_if_t<!HasOnDestroy<Comp>::value, bool> = true>
	constexpr static void BindOnDestroy( RegistryType& ) {}

	template<class Comp, class RegistryType, std::enable_if_t<HasOnDestroy<Comp>::value, bool> = true>
	constexpr static void BindOnDestroy( RegistryType& registry )
	{
		registry.on_destroy<Comp>().connect<&entt::invoke<&Comp::OnDestroy>>();
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
		return HasEditorUI<Comp>();
	}

	//////////////////////////////////////////////////////////////////////////
	// AddComponent
	//////////////////////////////////////////////////////////////////////////
	template <class Comp, class RegistryType, class EntityType>
	constexpr void AddComponent( RegistryType& registry, EntityType _entity )
	{
		if (!registry.template has<Comp>(_entity))
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