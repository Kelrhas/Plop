#pragma once

#include <ECS/LevelBase.h>

class SampleLevel : public Plop::LevelBase
{
public:
	virtual void Init() override;
	virtual void Update(Plop::TimeStep &_ts) override;

private:
};