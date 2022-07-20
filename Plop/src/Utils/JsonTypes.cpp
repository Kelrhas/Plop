#include "Plop_pch.h"

#include "JsonTypes.h"

namespace nlohmann
{
	///////////////////////////////////////////////////////////////////////////
	// GLM types

	/* glm::quat */
	void adl_serializer<glm::quat>::to_json(json &j, const glm::quat &_quat)
	{
		j	 = json();
		j[0] = _quat.x;
		j[1] = _quat.y;
		j[2] = _quat.z;
		j[3] = _quat.w;
	}

	void adl_serializer<glm::quat>::from_json(const json &j, glm::quat &_quat)
	{
		ASSERTM(j.size() == 4, "Json::Not enough parameter, wanted 4 got %llu", j.size());
		_quat.x = j[0];
		_quat.y = j[1];
		_quat.z = j[2];
		_quat.w = j[3];
	}


	///////////////////////////////////////////////////////////////////////////
	// Plop types

	/* GUID */
	void adl_serializer<Plop::GUID>::to_json(json &j, const Plop::GUID &_guid)
	{
		j = json();
		j = (uint64_t)_guid;
	}

	void adl_serializer<Plop::GUID>::from_json(const json &j, Plop::GUID &_guid) { _guid = (uint64_t)j; }

	/* Component_ParticleSystem::ParticleSpawnerPtr */
	void adl_serializer<Plop::Component_ParticleSystem::ParticleSpawnerPtr>::to_json(json &													   j,
																					 const Plop::Component_ParticleSystem::ParticleSpawnerPtr &_xSpawner)
	{
		j[_xSpawner->Name()] = _xSpawner->to_json();
	}

	/* Component_ParticleSystem::ParticleUpdaterPtr */
	void adl_serializer<Plop::Component_ParticleSystem::ParticleUpdaterPtr>::to_json(json &													   j,
																					 const Plop::Component_ParticleSystem::ParticleUpdaterPtr &_xUpdater)
	{
		j[_xUpdater->Name()] = _xUpdater->to_json();
	}
} // namespace nlohmann