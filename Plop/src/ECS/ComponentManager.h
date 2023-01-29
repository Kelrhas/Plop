#pragma once

#include <entt/core/fwd.hpp>
#include <entt/entity/registry.hpp>
#include <entt/meta/factory.hpp>
#include <json.hpp>

using json = nlohmann::json;

#include "Components/ComponentDefinition.h"

namespace Plop
{
	class ComponentManager
	{
		using EntityType = entt::entity;
		using Registry = entt::basic_registry<EntityType>;
		using ComponentTypeId = entt::id_type;
		using Json = nlohmann::json;

		static_assert(std::is_trivially_copyable_v<EntityType>);

	public:
		struct ComponentInfo
		{
			using Callback = std::function<void( Registry&, EntityType )>;
			using CallbackDuplicate = std::function<void( const Registry&, EntityType, Registry&, EntityType )>;
			using CallbackFromJson = std::function<void( Registry&, EntityType, const Json& )>;
			using CallbackToJson = std::function<Json( const Registry&, EntityType )>;


			const char* pName = nullptr;
			Callback funcEditorUI;
			std::function<bool(void)> funcCanAdd;
			std::function<bool(void)> funcCanRemove;
			std::function<bool(void)> funcCanEdit;
			Callback funcAdd;
			Callback funcRemove;
			CallbackDuplicate funcDuplicate;
			CallbackFromJson funcFromJson;
			CallbackToJson funcToJson;
			Callback funcAfterLoad;

		};

		template<typename Comp>
		static void RegisterComponent( const char* _pName );


		template<typename Comp>
		static void Test( Registry& _reg, EntityType& _e )
		{
			s_mapComponents.begin()->second.funcEditorUI( _reg, _e );
		}

		static void EditorUI( Registry& _reg, EntityType _e );
		static void FromJson( Registry& _reg, EntityType _e, const Json& _j);
		static void ToJson( const Registry& _reg, EntityType _e, Json& _j);
		static void AfterLoad(Registry &_reg, EntityType _e);
		static void DuplicateComponent( Registry& _reg, EntityType _entitySrc, EntityType _entityDest );

		template<typename Visitor>
		static void VisitAllComponents(Visitor &&_v);
		template<typename Visitor>
		static void ComponentsVisitor( Registry& _reg, EntityType _e, Visitor _v );

		static bool HasComponent( const Registry& _reg, EntityType _e, ComponentTypeId _id );

	private:


		template<typename Comp>
		static Comp& MetaGetComponent( Registry& _reg, EntityType _e );
		template<typename Comp>
		static bool MetaHasComponent( const Registry& _reg, EntityType _e );
		template<typename Comp>
		static Comp& MetaAddComponent( Registry& _reg, EntityType _e );
		//template<typename Comp>
		//static void MetaCloneComponent( const Registry& _regSrc, EntityType _eSrc, Registry &_regDst, const EntityType &_eDst);


		static std::unordered_map<ComponentTypeId, ComponentInfo> s_mapComponents;

	};



}

#include <entt/core/type_info.hpp>
#include <entt/meta/resolve.hpp>

namespace Plop
{

	template<typename Comp>
	void ComponentManager::RegisterComponent( const char* _pName )
	{
		constexpr auto id = entt::type_info<Comp>::id();
		ComponentInfo info;
		info.pName = _pName;
		info.funcCanEdit = CanEditComponent<Comp>;
		info.funcEditorUI = CallComponentEditorUI<Comp, Registry&, EntityType>;
		info.funcCanAdd = CanAddComponent<Comp>;
		info.funcCanRemove = CanRemoveComponent<Comp>;
		info.funcAdd = AddComponent<Comp, Registry&, EntityType>;
		info.funcRemove = RemoveComponent<Comp, Registry&, EntityType>;
		info.funcDuplicate = CallDuplicateComponent<Comp, Registry, EntityType>;
		info.funcFromJson = CallComponentFromJson<Comp, Registry&, EntityType, const Json&>;
		info.funcToJson = CallComponentToJson<Comp, Json, const Registry&, EntityType>;
		info.funcAfterLoad = CallComponentAfterLoad<Comp, Registry &, EntityType>;


		s_mapComponents.insert_or_assign( id, info );



		/// meta/reflection stuff

		auto factory = entt::meta<Comp>().type( entt::hashed_string( _pName ) ).prop("name"_hs, String(_pName));
		factory.func<&CloneAllRegistryComponents<Comp, Registry &>>("cloneAllComponents"_hs);
		factory.func<&CallDuplicateComponent<Comp, Registry, EntityType>>("clone"_hs);
		factory.func<&MetaGetComponent<Comp>, entt::as_ref_t>("get"_hs);
		factory.func<&MetaHasComponent<Comp>>("has"_hs);
		factory.func<&MetaAddComponent<Comp>, entt::as_ref_t>("add"_hs);

		/*if constexpr (std::is_invocable_v<Comp::SetupReflection>)
		{
			factory.func<&Comp::SetupReflection>("setup"_hs);
		}*/
	}

	template<typename Visitor> // [](const entt::id_type, const ComponentManager::ComponentInfo&){}
	void ComponentManager::VisitAllComponents(Visitor &&_v)
	{
		for (auto &[id, info] : s_mapComponents)
		{
			_v(id, info);
		}
	}

	template<typename Visitor>
	void ComponentManager::ComponentsVisitor( Registry& _reg, ComponentManager::EntityType _e, Visitor _v )
	{/*
		for (auto& [id, info] : s_mapComponents)
		{
			if (HasComponent( _reg, _e, id ))
			{
				_reg.visit( _e, [&_v]( const auto comp ) {
					const auto type = entt::resolve_type( comp );
					const auto any = type.func( "get"_hs ).invoke( {}, std::ref( _reg ), _e );
					_v( any );
				} );
			}
		}*/


		_reg.visit( _e, [&]( const auto compId ) {
			const auto type = entt::resolve_type( compId );
			if (type.func("has"_hs).invoke({}, std::ref(_reg), _e))
			{
				const entt::meta_any &compObj = type.func("get"_hs).invoke({}, std::ref(_reg), _e);
				_v(compObj);
			}
		} );
	}

	template<typename Comp>
	Comp& ComponentManager::MetaGetComponent( ComponentManager::Registry& _reg, EntityType _e ) {
		return _reg.get_or_emplace<Comp>( _e );
	}

	template<typename Comp>
	bool ComponentManager::MetaHasComponent( const ComponentManager::Registry& _reg, EntityType _e ) {
		return _reg.has<Comp>( _e );
	}

	template<typename Comp>
	Comp& ComponentManager::MetaAddComponent( ComponentManager::Registry& _reg, EntityType _e ) {
		return _reg.emplace<Comp>( _e );
	}


	//template<typename Comp>
	//void ComponentManager::MetaCloneComponent(const ComponentManager::Registry &_regSrc, const EntityType &_eSrc,
	//										  ComponentManager::Registry &_regDst, const EntityType &_eDst)
	//{
	//	const Comp &compSrc = _regSrc.get<Comp>(_eSrc);
	//	Comp &compDst = _regDst.get_or_emplace<Comp>(_eDst, compSrc);
	//}
}