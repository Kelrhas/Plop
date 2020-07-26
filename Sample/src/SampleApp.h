#pragma once

struct SampleConfig : public Plop::GameConfig
{

};

class SampleApp : public Plop::Application
{
public:
	SampleApp();

	Plop::GameConfig* CreateGameConfig() override;
};