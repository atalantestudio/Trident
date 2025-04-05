#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const ConfigurationFile& configurationFile) :
				Application("Sandbox", configurationFile)
			{
				registerLayer<DemoLayer>();
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Sandbox.tco");

		return new SandboxApplication(configurationFile);
	}
}