#pragma once

#include <glm/glm.hpp>
#include "Renderer/ParticleSystem.h"

#include <json.hpp>
using json = nlohmann::json;

namespace nlohmann {
	template<>
	struct adl_serializer<glm::mat4> {
		static void to_json( json& j, const glm::mat4& _matrix );
		static void from_json( const json& j, glm::mat4& _matrix );
	};
	template<>
	struct adl_serializer<glm::vec2> {
		static void to_json( json& j, const glm::vec2& _vec );
		static void from_json( const json& j, glm::vec2& _vec );
	};
	template<>
	struct adl_serializer<glm::vec3> {
		static void to_json( json& j, const glm::vec3& _vec );
		static void from_json( const json& j, glm::vec3& _vec );
	};
	template<>
	struct adl_serializer<glm::uvec2> {
		static void to_json( json& j, const glm::uvec2& _vec );
		static void from_json( const json& j, glm::uvec2& _vec );
	};
	template<>
	struct adl_serializer<glm::vec4> {
		static void to_json( json& j, const glm::vec4& _vec );
		static void from_json( const json& j, glm::vec4& _vec );
	};


	template<>
	struct adl_serializer<Plop::ParticleSystemComponent::ParticleSpawnerPtr> {
		static void to_json( json& j, const Plop::ParticleSystemComponent::ParticleSpawnerPtr& _xSpawner );
	};

	template<>
	struct adl_serializer<Plop::ParticleSystemComponent::ParticleUpdaterPtr> {
		static void to_json( json& j, const Plop::ParticleSystemComponent::ParticleUpdaterPtr& _xSpawner );
	};
}