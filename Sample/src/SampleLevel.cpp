#include "Config.h"

#include "SampleLevel.h"

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122 ?
#include "SampleComponents.h"

#include <ECS/Components/Component_Transform.h>

void SampleLevel::Init()
{
	LevelBase::Init();
}

void SampleLevel::Update(Plop::TimeStep &_ts)
{
	LevelBase::Update(_ts);


	auto view = m_ENTTRegistry.view<Plop::Component_Transform, Component_Rotating>();
	for (auto entity : view)
	{
		auto [transform, rotComp] = view.get<Plop::Component_Transform, Component_Rotating>(entity);

		glm::quat qRot = transform.GetLocalRotation();
		qRot		   = glm::rotate(qRot, rotComp.fSpeed * _ts.GetGameDeltaTime(), rotComp.vAxis);
		transform.SetLocalRotation(qRot);
	}
}