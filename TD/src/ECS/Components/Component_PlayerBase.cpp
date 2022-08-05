#include "TD_pch.h"

#include "Component_PlayerBase.h"

#include "ECS/Components/Component_Enemy.h"
#include "TDEvents.h"

#include <ECS/Components/Component_Physics.h>
#include <ECS/Components/Component_Transform.h>
#include <Events/EventDispatcher.h>
#include <Events/GameEvent.h>

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122

void Component_PlayerBase::OnHit(float _fDamage)
{
	fLife -= _fDamage;
	if (fLife <= 0.f)
	{
		Plop::GameEvent ev((S32)GameEventType::GameOver);
		Plop::EventDispatcher::SendEvent(ev);
	}
}

void Component_PlayerBase::EditorUI()
{
	ImGui::DragFloat("Life", &fLife, 0.1f, 1.f, FLT_MAX);
}

json Component_PlayerBase::ToJson() const
{
	json j;
	j["Life"] = fLife;
	return j;
}
void Component_PlayerBase::FromJson(const json &_j)
{
	if (_j.contains("Life"))
		fLife = _j["Life"];
}

namespace PlayerBaseSystem
{
	Component_PlayerBase *pPlayerBaseComp = nullptr;
		
	void OnStart(entt::registry &_registry)
	{
		_registry.view<Component_PlayerBase>().each([&](entt::entity _enttID, Component_PlayerBase &_baseComp) {
			ASSERTM(pPlayerBaseComp == nullptr, "PlayerBase already found, only one is needed");
			pPlayerBaseComp = &_baseComp;
			// TODO register some callback for when the entity/component is destroyed, so we dont keep an invalid pointer
			// or use a shared_pointer...
		});
		ASSERTM(pPlayerBaseComp != nullptr, "PlayerBase not found");
	}

	void OnUpdate(const Plop::TimeStep &_ts, entt::registry &_registry)
	{
		auto &viewEnemy = _registry.view<Component_Enemy, Plop::Component_AABBCollider, Plop::Component_Transform>();
		_registry.view<Component_PlayerBase, Plop::Component_AABBCollider>().each(
		  [&](entt::entity _baseEnttID, Component_PlayerBase &_baseComp, Plop::Component_AABBCollider &_baseCollider) {
			  viewEnemy.each([&](entt::entity					  _enemyEnttID,
								 Component_Enemy &				  _enemyComp,
								 Plop::Component_AABBCollider &	  _enemyCollider,
								 const Plop::Component_Transform &_enemyTransform) {
				  if (_baseCollider.IsColliding(_enemyCollider, _enemyTransform.GetWorldPosition()))
				  {
					  _baseComp.OnHit(_enemyComp.fDamageToBase);
					  _enemyComp.fLife = 0.f; // kill the enemy

					  Plop::GameEvent gameEvent((S32)GameEventType::LifeLost);
					  gameEvent.SetData(_enemyComp.fDamageToBase);
					  Plop::EventDispatcher::SendEvent(gameEvent);
				  }
			  });
		  });

		if (ImGui::Begin("PlayerBase"))
		{
			ImGui::Text("Life: %f", pPlayerBaseComp->fLife);
			ImGui::End();
		}
	}

	void OnStop(entt::registry &_registry) { pPlayerBaseComp = nullptr; }
} // namespace PlayerBaseSystem

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template<>
	void ComponentEditorWidget<Component_PlayerBase>(entt::registry &reg, entt::registry::entity_type e)
	{
		auto &comp = reg.get<Component_PlayerBase>(e);
		comp.EditorUI();
	}

	template<>
	json ComponentToJson<Component_PlayerBase>(entt::registry &reg, entt::registry::entity_type e)
	{
		auto &comp = reg.get<Component_PlayerBase>(e);
		return comp.ToJson();
	}

	template<>
	void ComponentFromJson<Component_PlayerBase>(entt::registry &reg, entt::registry::entity_type e, const json &_j)
	{
		auto &comp = reg.get_or_emplace<Component_PlayerBase>(e);
		comp.FromJson(_j);
	}
} // namespace MM
#endif