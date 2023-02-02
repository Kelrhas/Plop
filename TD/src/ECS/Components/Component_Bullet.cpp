#include "TD_pch.h"
#include "Component_Bullet.h"

static constexpr const char *JSON_SPEED = "Speed";

Component_Bullet::Component_Bullet( const Component_Bullet& _other )
{
	fSpeed = _other.fSpeed;
	vVelocity = _other.vVelocity;
	emitting = _other.emitting;
}

void Component_Bullet::EditorUI()
{
	ImGui::DragFloat("Speed", &fSpeed, 0.001f, 100.f);
}

json Component_Bullet::ToJson() const
{
	json j;
	j[JSON_SPEED] = fSpeed;
	return j;
}
void Component_Bullet::FromJson(const json &_j)
{
	if (_j.contains(JSON_SPEED))
		fSpeed = _j[JSON_SPEED];
}
