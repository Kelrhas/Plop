#include "PlopInclude.h"
#include "SampleLevel.h"

#pragma warning(disable:4267) // https://github.com/skypjack/entt/issues/122 ?
#include <ECS/BaseComponents.h>

#include "SampleComponents.h"


void SampleLevel::Update( Plop::TimeStep _ts )
{
	LevelBase::Update( _ts );


	auto& view = m_ENTTRegistry.view<Plop::TransformComponent, RotatingComponent>();
	for (auto entity : view)
	{
		auto& [transform, rotComp] = view.get<Plop::TransformComponent, RotatingComponent>( entity );
		
		glm::quat qRot = transform.GetLocalRotation();
		qRot = glm::rotate( qRot, rotComp.fSpeed * _ts.GetGameDeltaTime(), rotComp.vAxis );
		transform.SetLocalRotation( qRot );
	}
}