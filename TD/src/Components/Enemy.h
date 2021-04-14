#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>


struct EnemyComponent
{
	float fLife = 1.f;
	float fMoveSpeed = 0.2f;

	void Hit( float _fDamage );
	bool IsDead() const;


	Plop::Math::CatmullRomCurvePtr xPathCurve;
	float fPathPosition = 0.f;
};


namespace EnemySystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry );
};

namespace MM
{
	template <>	void ComponentEditorWidget<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<EnemyComponent>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
