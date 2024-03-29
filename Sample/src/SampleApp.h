#pragma once

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
	virtual StringPath			GetEngineDirectory() const override { return ".." / Plop::Application::GetEngineDirectory(); }
	virtual	StringPath			GetEditorDirectory() const override { return ".." / Plop::Application::GetEditorDirectory(); }


private:
	virtual StringPath			GetRootDirectory() const override { return StringPath("Sample"); }
	virtual Plop::GameConfig*	CreateGameConfig() final;
	virtual Plop::LevelBasePtr	CreateNewLevelPrivate() final;
	virtual void				RegisterComponents() final;
};