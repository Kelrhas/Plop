#pragma once

struct SampleConfig : public Plop::GameConfig
{

};

class SampleApp : public Plop::Application
{
public:
	SampleApp( const StringVec& _Arguments );
	SampleApp( const SampleApp& ) = delete;

			Plop::GameConfig*	CreateGameConfig() override;
	virtual	void				Init() override;

	virtual String				GetName() const override { return String("Sample"); }
	virtual StringPath			GetRootDirectory() const override { return StringPath( "Sample" ); }
};