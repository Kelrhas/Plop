#pragma once

struct TDConfig : public Plop::GameConfig
{

};

class TDApp : public Plop::Application
{
public:
	TDApp();

			Plop::GameConfig*	CreateGameConfig() override;
	virtual	void				Init() override;

	virtual String				GetName() override { return String( "Plop TD" ); }
};