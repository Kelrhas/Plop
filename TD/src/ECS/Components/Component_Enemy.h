#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>


struct Component_Enemy
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

#ifndef USE_COMPONENT_MGR
namespace MM
{
	template <>	void ComponentEditorWidget<Component_Enemy>( entt::registry& reg, entt::registry::entity_type e );
	template <>	json ComponentToJson<Component_Enemy>( entt::registry& reg, entt::registry::entity_type e );
	template <>	void ComponentFromJson<Component_Enemy>( entt::registry& reg, entt::registry::entity_type e, const json& _j );
}
#endif