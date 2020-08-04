#pragma once

#include <ApplicationLayer.h>
#include <Camera/OrthographicCamera.h>

class SampleLayer : public Plop::ApplicationLayer
{
public:
	virtual void OnRegistered() override;
	virtual void OnUnregistered() override {}
	virtual void OnUpdate(Plop::TimeStep& _timeStep) override;

	virtual uint8_t GetPriority() const override { return 10; }

private:
	Plop::OrthographicCamera* m_pCamera = nullptr;
};

