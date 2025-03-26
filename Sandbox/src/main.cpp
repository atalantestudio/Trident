#include "ProjectA.hpp"
#include "Engine/entry_point.hpp"

#include "Layer/SandboxLayer.hpp"
#include "Random.hpp"

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication(const ProjectA::ConfigurationFile& configurationFile) :
			ProjectA::Application("Sandbox", configurationFile)
		{
			registerLayer<SandboxLayer>();

			Random::initialize();
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	const ProjectA::ConfigurationFile configurationFile = ProjectA::Filesystem::load<ProjectA::ConfigurationFile>("Sandbox.tco");

	return new SandboxApplication(configurationFile);
}