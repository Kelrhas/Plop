#include "Sample_pch.h"

#include "SampleLevel.h"

#pragma warning(disable : 4267) // https://github.com/skypjack/entt/issues/122 ?
#include "SampleComponents.h"

#include <ECS/Components/Component_Transform.h>
#include <UI/IUIElement.h>

void SampleLevel::Init()
{
	LevelBase::Init();

	m_xRoot = std::make_unique<UI::Dialog>();
	m_xRoot->SetPosition(VEC2_0); // top left corner
	m_xRoot->SetSize(VEC2_1);	  // fullscreen
	
	m_xRoot->CreateElement<UI::Text>("Text");
}

void SampleLevel::Update(Plop::TimeStep &_ts)
{
	LevelBase::Update(_ts);


	auto &view = m_ENTTRegistry.view<Plop::Component_Transform, Component_Rotating>();
	for (auto entity : view)
	{
		auto &[transform, rotComp] = view.get<Plop::Component_Transform, Component_Rotating>(entity);

		glm::quat qRot = transform.GetLocalRotation();
		qRot		   = glm::rotate(qRot, rotComp.fSpeed * _ts.GetGameDeltaTime(), rotComp.vAxis);
		transform.SetLocalRotation(qRot);
	}

	m_xRoot->OnUpdate();
	m_xRoot->OnRender();
}