#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"
//#include "Layer/TextLayer.hpp"
//#include "Layer/Win32TestLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const WindowDescriptor& windowDescriptor) :
				Application(windowDescriptor)
			{
				//const TrueTypeFont font("Asset/Font/test.ttf");

				registerLayer<DemoLayer>(/*font*/);
				//registerLayer<TextLayer>(font);
				//registerLayer<Win32TestLayer>();
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		WindowDescriptor windowDescriptor{};
		windowDescriptor.name = PROJECT_NAME;
		windowDescriptor.width = 800;
		windowDescriptor.height = 800;

		return new SandboxApplication(windowDescriptor);
	}
}