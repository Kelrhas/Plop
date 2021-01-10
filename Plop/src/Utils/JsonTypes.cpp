#include "Plop_pch.h"
#include "JsonTypes.h"

namespace nlohmann
{
	/* glm::mat4*/
	void adl_serializer<glm::mat4>::to_json( json& j, const glm::mat4& _matrix )
	{
		j = json();
		for (int x = 0; x < 4; ++x)
			for (int y = 0; y < 4; ++y)
				j[x * 4 + y] = _matrix[x][y];
	}

	void adl_serializer<glm::mat4>::from_json( const json& j, glm::mat4& _matrix )
	{
		for (int x = 0; x * 4 < j.size() && x < 4; ++x)
			for (int y = 0; x * 4 + y < j.size() && y < 4; ++y)
				j[x * 4 + y].get_to( _matrix[x][y] );
	}

	/* glm::vec2*/
	void adl_serializer<glm::vec2>::to_json( json& j, const glm::vec2& _vec )
	{
		j = json();
		for (int x = 0; x < 2; ++x)
			j[x] = _vec[x];
	}

	void adl_serializer<glm::vec2>::from_json( const json& j, glm::vec2& _vec )
	{
		for (int x = 0; x < j.size() && x < 2; ++x)
			j[x].get_to( _vec[x] );
	}

	/* glm::vec3*/
	void adl_serializer<glm::vec3>::to_json( json& j, const glm::vec3& _vec )
	{
		j = json();
		for (int x = 0; x < 3; ++x)
			j[x] = _vec[x];
	}

	void adl_serializer<glm::vec3>::from_json( const json& j, glm::vec3& _vec )
	{
		for (int x = 0; x < j.size() && x < 3; ++x)
			j[x].get_to( _vec[x] );
	}

	/* glm::uvec2*/
	void adl_serializer<glm::uvec2>::to_json( json& j, const glm::uvec2& _vec )
	{
		j = json();
		for (int x = 0; x < 2; ++x)
			j[x] = _vec[x];
	}

	void adl_serializer<glm::uvec2>::from_json( const json& j, glm::uvec2& _vec )
	{
		for (int x = 0; x < j.size() && x < 2; ++x)
			j[x].get_to( _vec[x] );
	}

	/* glm::vec4*/
	void adl_serializer<glm::vec4>::to_json( json& j, const glm::vec4& _vec )
	{
		j = json();
		for (int x = 0; x < 4; ++x)
			j[x] = _vec[x];
	}

	void adl_serializer<glm::vec4>::from_json( const json& j, glm::vec4& _vec )
	{
		for (int x = 0; x < j.size() && x < 4; ++x)
			j[x].get_to( _vec[x] );
	}

	/* ParticleSystemComponent::ParticleSpawnerPtr*/
	void adl_serializer<Plop::ParticleSystemComponent::ParticleSpawnerPtr>::to_json( json& j, const Plop::ParticleSystemComponent::ParticleSpawnerPtr& _xSpawner )
	{
		j[_xSpawner->Name()] = _xSpawner->to_json();
	}

	/* ParticleSystemComponent::ParticleUpdaterPtr*/
	void adl_serializer<Plop::ParticleSystemComponent::ParticleUpdaterPtr>::to_json( json& j, const Plop::ParticleSystemComponent::ParticleUpdaterPtr& _xUpdater )
	{
		j[_xUpdater->Name()] = _xUpdater->to_json();
	}
}