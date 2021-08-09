#pragma once

#include <ApplicationLayer.h>

#include "ECS/TDLevel.h"

//////////////////////////////////////////////////////////////////////////
// WorldLayer
class WorldLayer : public Plop::ApplicationLayer
{
public:
	virtual void OnRegistered() override;
	virtual void OnUnregistered() override;
	virtual void OnUpdate(Plop::TimeStep& _ts ) override;

	virtual uint8_t GetPriority() const override { return 20; }

private:
};

