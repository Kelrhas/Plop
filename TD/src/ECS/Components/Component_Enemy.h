#pragma once

#include <imgui_entt_entity_editor.hpp>

#include <Math/Curves.h>


struct Component_Enemy
{
	float fLife = 1.f;
	float fMoveSpeed = 2.2f;
	float fDamageToBase = 1.f;

	void Hit( float _fDamage );
	bool IsDead() const;


	Plop::Math::CatmullRomCurvePtr xPathCurve;
	float fPathPosition = 0.f;

	

	void EditorUI();
	json ToJson() const;
	void FromJson(const json &_j);
};


namespace EnemySystem
{
	void OnUpdate( const Plop::TimeStep& _ts, entt::registry& _registry );
};