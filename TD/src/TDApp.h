#pragma once

struct TDConfig : public Plop::GameConfig
{

};

class TDApp : public Plop::Application
{
public:
	TDApp( const StringVec& _Arguments );

	virtual	void				Init() override;
	virtual String				GetName() const override { return String( "Plop TD" ); }
	virtual StringPath			GetEngineDirectory() const override { return ".." / Plop::Application::GetEngineDirectory(); }
	virtual	StringPath			GetEditorDirectory() const override { return ".." / Plop::Application::GetEditorDirectory(); }

private:
	virtual StringPath			GetRootDirectory() const override { return StringPath("TD"); }
	virtual Plop::GameConfig*	CreateGameConfig() override;
	virtual Plop::LevelBasePtr	CreateNewLevelPrivate() override;
	virtual void				RegisterComponents() override;
};