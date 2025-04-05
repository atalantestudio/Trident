#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const ConfigurationFile& configurationFile) :
				Application("Sandbox", configurationFile)
			{
				const TTFFile ttfFile = Filesystem::load<TTFFile>("assets/fonts/test.ttf");

				registerLayer<DemoLayer>(ttfFile);
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Sandbox.tco");

		return new SandboxApplication(configurationFile);
	}
}