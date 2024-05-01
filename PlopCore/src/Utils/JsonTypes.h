#pragma once

#include "ECS/Components/Component_ParticleSystem.h"
#include "ECS/PrefabManager.h"
#include "Utils/GUID.h"

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlohmann
{
	///////////////////////////////////////////////////////////////////////////
	// GLM types


	template<glm::length_t Row, glm::length_t Col, typename T, glm::qualifier Q>
	struct adl_serializer<glm::mat<Row, Col, T, Q>>
	{
		static_assert(Row == Col, "Check the case Row != Col");
		using Mat = glm::mat<Row, Col, T, Q>;

		static void to_json(json &j, const Mat &_matrix)
		{
			j = json();
			for (int x = 0; x < Row; ++x)
				for (int y = 0; y < Col; ++y)
					j[x * Col + y] = _matrix[x][y];
		}
		static void from_json(const json &j, Mat &_matrix)
		{
			for (int x = 0; x * 4 < j.size() && x < Row; ++x)
				for (int y = 0; x * Col + y < j.size() && y < Col; ++y)
					j[x * Col + y].get_to(_matrix[x][y]);
		}
	};

	template<glm::length_t L, typename T, glm::qualifier Q>
	struct adl_serializer<glm::vec<L, T, Q>>
	{
		using Vec = glm::vec<L, T, Q>;
		static void to_json(json &_j, const Vec &_vec)
		{
			_j = json();
			for (int x = 0; x < L; ++x)
				_j[x] = _vec[x];
		}
		static void from_json(const json &_j, Vec &_vec)
		{
			for (int x = 0; x < _j.size() && x < L; ++x)
				_j[x].get_to(_vec[x]);
		}
	};

	template<typename T, glm::qualifier Q>
	struct adl_serializer<glm::qua<T, Q>>
	{
		static_assert(std::is_same_v<decltype(glm::qua<T, Q>), glm::quat>, "we should not be using any other quaternion type than the builtin glm::quat");
	};

	template<>
	struct adl_serializer<glm::quat>
	{
		static void to_json(json &_j, const glm::quat &_quat);
		static void from_json(const json &_j, glm::quat &_quat);
	};


	///////////////////////////////////////////////////////////////////////////
	// Plop types

	template<>
	struct adl_serializer<Plop::GUID>
	{
		static void to_json(json &j, const Plop::GUID &_guid);
		static void from_json(const json &j, Plop::GUID &_guid);
	};

	template<>
	struct adl_serializer<Plop::PrefabHandle>
	{
		static void to_json(json &j, const Plop::PrefabHandle &_hPrefab);
		static void from_json(const json &j, Plop::PrefabHandle &_hPrefab);
	};

	template<>
	struct adl_serializer<Plop::Component_ParticleSystem::ParticleSpawnerPtr>
	{
		static void to_json(json &j, const Plop::Component_ParticleSystem::ParticleSpawnerPtr &_xSpawner);
	};

	template<>
	struct adl_serializer<Plop::Component_ParticleSystem::ParticleUpdaterPtr>
	{
		static void to_json(json &j, const Plop::Component_ParticleSystem::ParticleUpdaterPtr &_xSpawner);
	};
} // namespace nlohmann