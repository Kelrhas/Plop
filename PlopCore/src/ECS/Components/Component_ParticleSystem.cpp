#include "Config.h"

#include "Component_ParticleSystem.h"

#include "Application.h"
#include "ECS/Components/Component_Transform.h"
#include "ECS/ECSHelper.h"
#include "ECS/Entity.h"
#include "ECS/LevelBase.h"
#include "ECS/ParticleSpawners.h"
#include "Renderer/Renderer.h"
#include "Utils/JsonTypes.h"
#include "Utils/imgui_custom.h"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

static constexpr const char *JSON_CAPACITY		= "Capacity";
static constexpr const char *JSON_AUTOSPAWNRATE = "AutoSpawnRate";
static constexpr const char *JSON_BURSTMODE		= "BurstMode";
static constexpr const char *JSON_SPAWNERS		= "Spawners";
static constexpr const char *JSON_UPDATERS		= "Updaters";

namespace
{
	struct SpawnerHandlerAbstract // type erasure
	{
		virtual Plop::Component_ParticleSystem::ParticleSpawnerPtr Instantiate() const = 0;
		virtual const char *const								   Name() const		   = 0;
	};
	template<typename Spawner>
	struct SpawnerHandler : SpawnerHandlerAbstract
	{
		Plop::Component_ParticleSystem::ParticleSpawnerPtr Instantiate() const final { return std::make_shared<Spawner>(); }
		virtual const char *const						   Name() const final { return Spawner::StaticName(); }
	};

	// TODO: convert to constexpr std::array once using c++20
	static SpawnerHandlerAbstract *spawnerHandlerList[] = {
		new SpawnerHandler<Plop::Particle::SpawnLife>(),	  new SpawnerHandler<Plop::Particle::SpawnShapeCircle>(),
		new SpawnerHandler<Plop::Particle::SpawnShapeDisk>(), new SpawnerHandler<Plop::Particle::SpawnShapeArc>(),
		new SpawnerHandler<Plop::Particle::SpawnShapeRect>(), new SpawnerHandler<Plop::Particle::SpawnColor>(),
		new SpawnerHandler<Plop::Particle::SpawnSize>(),	  new SpawnerHandler<Plop::Particle::SpawnRadialSpeed>(),
	};

	struct UpdaterHandlerAbstract // type erasure
	{
		virtual Plop::Component_ParticleSystem::ParticleUpdaterPtr Instantiate() const = 0;
		virtual const char *const								   Name() const		   = 0;
	};
	template<typename Updater>
	struct UpdaterHandler : UpdaterHandlerAbstract
	{
		Plop::Component_ParticleSystem::ParticleUpdaterPtr Instantiate() const final { return std::make_shared<Updater>(); }
		virtual const char *const						   Name() const final { return Updater::StaticName(); }
	};

	// TODO: convert to constexpr std::array once using c++20
	static UpdaterHandlerAbstract *updaterHandlerList[] = {
		new UpdaterHandler<Plop::Particle::UpdatePositionFromSpeed>(),
		new UpdaterHandler<Plop::Particle::UpdateColorFromLifetime>(),
	};
} // namespace


namespace Plop
{
	Component_ParticleSystem::Component_ParticleSystem(size_t _iMaxParticles) : m_iParticleCapacity(std::max<size_t>(_iMaxParticles, 1))
	{
		m_pParticles = NEW ParticleData[m_iParticleCapacity];
	}

	Component_ParticleSystem::Component_ParticleSystem(const Component_ParticleSystem &_other) noexcept
	{
		*this = _other;
	}

	Component_ParticleSystem::Component_ParticleSystem(Component_ParticleSystem &&_other) noexcept
	{
		*this = std::move(_other);
	}

	Component_ParticleSystem::~Component_ParticleSystem()
	{
		delete[] m_pParticles;
	}

	Component_ParticleSystem &Component_ParticleSystem::operator=(const Component_ParticleSystem &_other) noexcept
	{
		if (m_pParticles)
			delete[] m_pParticles;
		m_pParticles = NEW ParticleData[_other.m_iParticleCapacity];
		memcpy(m_pParticles, _other.m_pParticles, sizeof(ParticleData) * _other.m_iActiveParticleCount); // no need to copy inactive particles
		m_iParticleCapacity	   = _other.m_iParticleCapacity;
		m_iActiveParticleCount = 0; // reset particles used
		m_fAutoSpawnRate	   = _other.m_fAutoSpawnRate;
		m_fAutoSpawnRemainder  = _other.m_fAutoSpawnRemainder;
		m_rand				   = _other.m_rand;
		m_vecSpawners.clear();
		m_vecSpawners.reserve(_other.m_vecSpawners.size());
		for (const auto &spawner : _other.m_vecSpawners)
		{
			m_vecSpawners.push_back(spawner->Clone());
		}
		m_vecUpdaters.clear();
		m_vecUpdaters.reserve(_other.m_vecUpdaters.size());
		for (const auto &updater : _other.m_vecUpdaters)
		{
			m_vecUpdaters.push_back(updater->Clone());
		}
		m_vBasePosition = _other.m_vBasePosition;
		m_bBurst		= _other.m_bBurst;

		return *this;
	}

	Component_ParticleSystem &Component_ParticleSystem::operator=(Component_ParticleSystem &&_other) noexcept
	{
		if (m_pParticles)
			delete[] m_pParticles;
		m_pParticles		   = _other.m_pParticles;
		m_iParticleCapacity	   = _other.m_iParticleCapacity;
		m_iActiveParticleCount = _other.m_iActiveParticleCount;
		m_fAutoSpawnRate	   = _other.m_fAutoSpawnRate;
		m_fAutoSpawnRemainder  = _other.m_fAutoSpawnRemainder;
		m_rand				   = _other.m_rand;
		m_vecSpawners		   = std::move(_other.m_vecSpawners);
		m_vecUpdaters		   = std::move(_other.m_vecUpdaters);
		m_vBasePosition		   = _other.m_vBasePosition;
		m_bBurst			   = _other.m_bBurst;

		_other.m_pParticles		   = nullptr;
		_other.m_iParticleCapacity = 0;
		_other.Clear();

		return *this;
	}

	void Component_ParticleSystem::Spawn(size_t _iNbParticle)
	{
		PROFILING_FUNCTION();

		if (m_iActiveParticleCount + _iNbParticle > m_iParticleCapacity)
		{
			// TODO: expand the pool
			_iNbParticle = m_iParticleCapacity - m_iActiveParticleCount;
		}

		auto xLevel	   = Application::GetCurrentLevel().lock();
		auto owner	   = GetComponentOwner(xLevel->GetEntityRegistry(), *this);
		m_mCachedWorld = owner ? owner.GetComponent<Component_Transform>().GetWorldMatrix() : glm::identity<glm::mat4>();

		glm::vec3 vBasePosition = m_mCachedWorld[3];
		while (m_iActiveParticleCount < m_iParticleCapacity && _iNbParticle > 0)
		{
			ParticleData &p = m_pParticles[m_iActiveParticleCount];
			p.vPosition		= vBasePosition;
			for (auto &spawner : m_vecSpawners)
			{
				spawner->Spawn(&p, *this);
			}

			--_iNbParticle;
			if (p.fRemainingLife > 0.f) // life has been correctly set
				++m_iActiveParticleCount;
		}
	}

	void Component_ParticleSystem::Update(const TimeStep &_ts)
	{
		PROFILING_FUNCTION();

		float fDT = _ts.GetGameDeltaTime();

		if (m_bBurst)
		{
			if (m_iActiveParticleCount == 0)
				Spawn(m_iParticleCapacity);
		}
		else if (m_fAutoSpawnRate > 0.f)
		{
			float  fNbParticles = (m_fAutoSpawnRate)*_ts.GetGameDeltaTime() + m_fAutoSpawnRemainder;
			size_t iNbParticles = (size_t)fNbParticles;
			Spawn(iNbParticles);
			m_fAutoSpawnRemainder = fNbParticles - iNbParticles;
		}

		int iNbActive = 0;
		for (size_t i = 0; i < m_iActiveParticleCount;)
		{
			if (m_pParticles[i].fRemainingLife > 0.f)
			{
				ParticleData &p = m_pParticles[i];

				p.fRemainingLife -= fDT;
				p.fLifeRatio = 1.f - std::clamp(p.fRemainingLife / p.fLifeTime, 0.f, 1.f);

				if (p.fRemainingLife <= 0.f)
				{
					// swap with the last active one
					m_pParticles[i] = m_pParticles[m_iActiveParticleCount - 1];
					--m_iActiveParticleCount;
					continue;
				}

				for (auto &updater : m_vecUpdaters)
				{
					updater->Update(&p, fDT);
				}

				// TODO: use a proper shader with one draw call, possibly point primitives etc.
				Renderer::DrawQuad(p.vColor, p.vPosition, p.vSize);
			}

			++i; // we can now go to the next particle
		}

		if (m_bBurst && m_bDestroyAfterBurst && m_iActiveParticleCount == 0)
		{
			if (!Application::Get()->GetEditor().IsEditing())
			{
				LevelBasePtr xLevel = Application::GetCurrentLevel().lock();
				auto		 owner	= GetComponentOwner(xLevel->GetEntityRegistry(), *this);
				xLevel->DestroyEntity(owner);
			}
		}
	}

	void Component_ParticleSystem::AddSpawner(const ParticleSpawnerPtr &_xSpawner)
	{
		m_vecSpawners.push_back(_xSpawner);
	}

	void Component_ParticleSystem::AddUpdater(const ParticleUpdaterPtr &_xUpdater)
	{
		m_vecUpdaters.push_back(_xUpdater);
	}

	void Component_ParticleSystem::Clear()
	{
		m_vecSpawners.clear();
		m_vecUpdaters.clear();
		m_fAutoSpawnRate = 0.f;
		ResetSystem();
	}

	void Component_ParticleSystem::ResetSystem()
	{
		for (size_t i = 0; i < m_iParticleCapacity; ++i)
		{
			m_pParticles[i].fRemainingLife = 0.f;
		}
		m_iActiveParticleCount = 0;
		m_fAutoSpawnRemainder  = 0;
	}

	void Component_ParticleSystem::SetMaxNbParticles(size_t _iMaxParticles)
	{
		if (_iMaxParticles != m_iParticleCapacity)
		{
			ParticleData *pNewArray = NEW ParticleData[_iMaxParticles];

			memcpy(pNewArray, m_pParticles, std::min(_iMaxParticles, m_iParticleCapacity) * sizeof(ParticleData));
			m_iParticleCapacity = _iMaxParticles;
			delete[] m_pParticles;
			m_pParticles = pNewArray;

			m_iActiveParticleCount = std::min(m_iActiveParticleCount, m_iParticleCapacity);
			for (size_t i = 0; i < m_iParticleCapacity; ++i)
			{
				if (m_pParticles[i].fRemainingLife < 0.f)
				{
					m_iActiveParticleCount = i;
					break;
				}
			}
		}
	}


	void Component_ParticleSystem::EditorUI()
	{
		Entity owner = GetComponentOwner(Application::GetCurrentLevel().lock()->GetEntityRegistry(), *this);

		float fSpawnRate = GetAutoSpawnRate();
		if (ImGui::DragFloat("Auto spawn rate", &fSpawnRate, 0.1f, 0.f, 100000.f))
			SetAutoSpawnRate(fSpawnRate);

		size_t	  iMaxParticle = GetMaxNbParticles();
		const int iNbChar	   = (int)std::log10(iMaxParticle) + 1;
		ImGui::Text("%*llu/%llu alive particles", iNbChar, GetNbActiveParticles(), iMaxParticle);

		static size_t iNewSize = iMaxParticle;
		if (ImGui::Custom::DragBufferSize("Max particles", &iNewSize, iMaxParticle, 1.f, 0xFFFFFFFF))
		{
			SetMaxNbParticles(iNewSize);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Checkbox("Burst", &m_bBurst);
		if (!m_bBurst)
			ImGui::DragFloat("Spawn rate", &m_fAutoSpawnRate);

		ImGui::Text("Spawners");
		{
			ImGui::PushID("Spawners");
			ImGui::SameLine();
			if (ImGui::SmallButton("+"))
			{
				ImGui::OpenPopup("NewSpawner");
			}

			if (ImGui::BeginPopup("NewSpawner"))
			{
				ImGui::TextUnformatted("Spawner");
				ImGui::Separator();

				for (auto &pHdl : spawnerHandlerList)
				{
					if (ImGui::Selectable(pHdl->Name()))
					{
						AddSpawner(pHdl->Instantiate());
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}


			auto &spawners = GetSpawners();
			ImGui::Indent();
			for (auto it = spawners.begin(); it != spawners.end();)
			{
				auto &spawner = *it;
				ImGui::PushID(spawner->Name());

				bool bKeep = true;
				if (ImGui::CollapsingHeader(spawner->Name(), &bKeep))
				{
					ImGui::Indent(20.f);
					spawner->Editor(owner, *this);
					ImGui::Unindent(20.f);
				}

				if (!bKeep)
				{
					it = spawners.erase(it);
				}
				else
					++it;

				ImGui::PopID();
			}
			ImGui::Unindent();
			ImGui::PopID();
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Text("Updaters");
		{
			ImGui::PushID("Updaters");
			ImGui::SameLine();
			if (ImGui::SmallButton("+"))
			{
				ImGui::OpenPopup("NewUpdater");
			}

			if (ImGui::BeginPopup("NewUpdater"))
			{
				ImGui::TextUnformatted("Updater");
				ImGui::Separator();

				for (auto &pHdl : updaterHandlerList)
				{
					if (ImGui::Selectable(pHdl->Name()))
					{
						AddUpdater(pHdl->Instantiate());
						ImGui::CloseCurrentPopup();
					}
				}

				ImGui::EndPopup();
			}

			auto &updaters = GetUpdaters();
			ImGui::Indent();
			for (auto it = updaters.begin(); it != updaters.end();)
			{
				auto &updater = *it;
				ImGui::PushID(updater->Name());

				bool bKeep = true;
				if (ImGui::CollapsingHeader(updater->Name(), &bKeep))
				{
					ImGui::Indent(20.f);
					updater->Editor(owner, *this);
					ImGui::Unindent(20.f);
				}

				if (!bKeep)
				{
					it = updaters.erase(it);
				}
				else
					++it;

				ImGui::PopID();
			}
			ImGui::Unindent();
			ImGui::PopID();
		}
	}

	json Component_ParticleSystem::ToJson() const
	{
		json j;
		j[JSON_CAPACITY]   = m_iParticleCapacity;
		j[JSON_AUTOSPAWNRATE] = GetAutoSpawnRate();
		j[JSON_BURSTMODE]	   = m_bBurst;
		j[JSON_SPAWNERS]	   = GetSpawners();
		j[JSON_UPDATERS]	   = GetUpdaters();

		return j;
	}

	void Component_ParticleSystem::FromJson(const json &_j)
	{
		if (_j.contains(JSON_CAPACITY))
			SetMaxNbParticles(_j[JSON_CAPACITY]);

		if (_j.contains(JSON_AUTOSPAWNRATE))
			SetAutoSpawnRate(_j[JSON_AUTOSPAWNRATE]);

		if (_j.contains(JSON_BURSTMODE))
			m_bBurst = _j[JSON_BURSTMODE];

		if (_j.contains(JSON_SPAWNERS))
		{
			for (auto &j : _j[JSON_SPAWNERS])
			{
				ASSERTM(j.is_object(), "Not an object");
				for (auto [key, value] : j.items())
				{
					for (auto &pHdl : spawnerHandlerList)
					{
						if (pHdl->Name() == key)
						{
							auto xSpawner = pHdl->Instantiate();
							xSpawner->from_json(value);
							AddSpawner(xSpawner);
							break;
						}
					}
				}
			}
		}
		if (_j.contains(JSON_UPDATERS))
		{
			for (auto &j : _j[JSON_UPDATERS])
			{
				ASSERTM(j.is_object(), "Not an object");
				for (auto [key, value] : j.items())
				{
					for (auto &pHdl : updaterHandlerList)
					{
						if (pHdl->Name() == key)
						{
							auto xUpdater = pHdl->Instantiate();
							xUpdater->from_json(value);
							AddUpdater(xUpdater);
							break;
						}
					}
				}
			}
		}
	}
} // namespace Plop
