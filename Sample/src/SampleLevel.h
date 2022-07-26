#pragma once

#include <ECS/LevelBase.h>
#include <UI/Dialog.h>
#include <UI/UIForward.h>

class SampleLevel : public Plop::LevelBase
{
public:
	virtual void Init() override;
	virtual void Update(Plop::TimeStep &_ts) override;

private:
	Plop::UI::DialogUPtr m_xRoot;
};