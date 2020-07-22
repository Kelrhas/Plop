#include <PlopInclude.h>


class SampleApp : public Plop::Application
{
public:

};



Plop::Application* Plop::CreateApplication()
{
	return new SampleApp();
}