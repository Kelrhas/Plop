// for the license, see the end of the file
#pragma once

#ifndef USE_COMPONENT_MGR



#include <map>
#include <set>

#pragma warning(disable:4307) // https://github.com/skypjack/entt/issues/121
#include <entt/entity/fwd.hpp>
#include <imgui.h>

#include <json.hpp>

using json = nlohmann::json;

namespace MM {

template <class Component, class EntityType>
void ComponentEditorWidget(entt::basic_registry<EntityType>& registry, EntityType entity) {}

template <class Component, class EntityType>
void ComponentAddAction(entt::basic_registry<EntityType>& registry, EntityType entity)
{
	registry.template emplace<Component>(entity);
}

template <class Component, class EntityType>
void ComponentRemoveAction(entt::basic_registry<EntityType>& registry, EntityType entity)
{
	registry.template remove<Component>(entity);
}

template <class Component, class EntityType>
void ComponentDuplicateAction( entt::basic_registry<EntityType>& registry, EntityType entitySrc, EntityType entityDest )
{
	if (registry.has<Component>( entitySrc ))
	{
		auto& compDest = registry.template get_or_emplace<Component>( entityDest );
		auto& compSrc = registry.template get<Component>( entitySrc );
		compDest = compSrc;
		int a = 10;
	}
}

template <class Component, class EntityType>
json ComponentToJson( entt::basic_registry<EntityType>& registry, EntityType entity )
{
	json j;
	j.push_back( "NO SERIALISATION" );
	return j;
}

template <class Component, class EntityType>
void ComponentFromJson( entt::basic_registry<EntityType>& registry, EntityType entity, const json& _j )
{
	if (!registry.has<Component>( entity ))
		registry.emplace<Component>( entity );
}

template <class EntityType>
class EntityEditor {
public:
	using Registry = entt::basic_registry<EntityType>;

	struct ComponentInfo {
		using Callback = std::function<void(Registry&, EntityType)>;
		using CallbackDuplicate = std::function<void(Registry&, EntityType, EntityType)>;
		using CallbackToJson = std::function<json(Registry&, EntityType)>;
		using CallbackFromJson = std::function<void(Registry&, EntityType, const json&)>;
		std::string name;
		Callback widget, create, destroy;
		CallbackDuplicate duplicate;
		CallbackToJson tojson;
		CallbackFromJson fromjson;
	};

//private:
	using ComponentTypeID = ENTT_ID_TYPE;

	std::map<ComponentTypeID, ComponentInfo> component_infos;

	bool entityHasComponent(Registry& registry, EntityType& entity, ComponentTypeID type_id)
	{
		ComponentTypeID type[] = { type_id };
		return registry.runtime_view(std::cbegin(type), std::cend(type)).contains(entity);
	}

public:
	template <class Component>
	ComponentInfo& registerComponent(const ComponentInfo& component_info)
	{
		auto index = entt::type_info<Component>::id();
		auto [it, insert_result] = component_infos.insert_or_assign(index, component_info);
		assert(insert_result);
		return std::get<ComponentInfo>(*it);
	}

	template <class Component>
	ComponentInfo& registerComponent(const std::string& name, typename ComponentInfo::Callback widget)
	{
		return registerComponent<Component>(ComponentInfo{
			name,
			widget,
			ComponentAddAction<Component, EntityType>,
			ComponentRemoveAction<Component, EntityType>,
			ComponentDuplicateAction<Component, EntityType>,
			ComponentToJson<Component, EntityType>,
			ComponentFromJson<Component, EntityType>,
		});
	}

	template <class Component>
	ComponentInfo& registerComponent(const std::string& name)
	{
		return registerComponent<Component>(name, ComponentEditorWidget<Component, EntityType>);
	}

	bool render(Registry& registry, EntityType& e)
	{
		bool show_window = true;
		if (show_window) {
			ImGui::SetNextWindowSizeConstraints( ImVec2( 450, 600 ), ImVec2( -1, -1 ) );
			if (ImGui::Begin("Entity Editor old", &show_window)) {
				ImGui::TextUnformatted("Editing:");
				ImGui::SameLine();

				if (registry.valid(e))
				{
					if (registry.has<Plop::Component_Name>( e ))
						ImGui::Text( registry.get<Plop::Component_Name>( e ).sName.c_str() );
					else
						ImGui::Text("ID: %d", entt::to_integral(e));
				} else {
					ImGui::Text("Invalid Entity");
				}

				/*if (ImGui::Button("New Entity")) {
					e = registry.create();
				}*/

				ImGui::Separator();

				if (registry.valid(e)) {
					ImGui::PushID(entt::to_integral(e));
					std::map<ComponentTypeID, ComponentInfo> has_not;
					for (auto& [component_type_id, ci] : component_infos) {
						if (entityHasComponent(registry, e, component_type_id)) {
							ImGui::PushID(component_type_id);
							if (ImGui::Button("-")) {
								ci.destroy(registry, e);
								ImGui::PopID();
								continue; // early out to prevent access to deleted data
							} else {
								ImGui::SameLine();
							}

							if (ImGui::CollapsingHeader(ci.name.c_str())) {
								ImGui::Indent(30.f);
								ImGui::PushID("Widget");
								ci.widget(registry, e);
								ImGui::PopID();
								ImGui::Unindent(30.f);
							}
							ImGui::PopID();
						} else {
							has_not[component_type_id] = ci;
						}
					}

					if (!has_not.empty()) {
						if (ImGui::Button("+ Add Component")) {
							ImGui::OpenPopup("Add Component");
						}

						if (ImGui::BeginPopup("Add Component")) {
							ImGui::TextUnformatted("Available:");
							ImGui::Separator();

							for (auto& [component_type_id, ci] : has_not) {
								ImGui::PushID(component_type_id);
								if (ImGui::Selectable(ci.name.c_str())) {
									ci.create(registry, e);
								}
								ImGui::PopID();
							}
							ImGui::EndPopup();
						}
					}
					ImGui::PopID();
				}
			}
			ImGui::End();
		}

		return show_window;
	}
};

} // MM

// MIT License

// Copyright (c) 2020 Erik Scholz, Gnik Droy

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#endif