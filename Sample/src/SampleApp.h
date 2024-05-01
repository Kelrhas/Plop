#pragma once

#include "Application.h"

struct SampleConfig : public Plop::GameConfig
{

};

class SampleApp : public Plop::Application
{
public:
	SampleApp( const StringVec& _Arguments );
	SampleApp( const SampleApp& ) = delete;

	virtual	void				Init() override;

	virtual String				GetName() const override { return String("Sample"); }


private:
	virtual StringPath			GetRootDirectory() const override { return StringPath("Sample"); }
	virtual Plop::GameConfig*	CreateGameConfig() final;
	virtual Plop::LevelBasePtr	CreateNewLevelPrivate() final;
	virtual void				RegisterComponents() final;
};