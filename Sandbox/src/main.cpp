#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"
//#include "Layer/TextLayer.hpp"
#include "Layer/Win32TestLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const WindowDescriptor& windowDescriptor/*, const ConfigurationFile& configurationFile*/) :
				Application(windowDescriptor/*, configurationFile*/)
			{
				//const TrueTypeFont font("assets/fonts/test.ttf");

				//registerLayer<DemoLayer>(/*font*/);
				//registerLayer<TextLayer>(font);
				registerLayer<Win32TestLayer>();
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		//const ConfigurationFile configurationFile = Filesystem::load<ConfigurationFile>("Sandbox.tco");

		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = "Sandbox";
		windowDescriptor.width = 800;
		windowDescriptor.height = 800;

		return new SandboxApplication(windowDescriptor/*, configurationFile*/);
	}
}