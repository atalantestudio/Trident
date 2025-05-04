#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

//#include "Layer/DemoLayer.hpp"
#include "Layer/TextLayer.hpp"
//#include "Layer/Win32TestLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const ApplicationDescriptor& applicationDescriptor) :
				Application(applicationDescriptor)
			{
				const TrueTypeFont font("Asset/Font/test.ttf");

				//registerLayer<DemoLayer>(/*font*/);
				registerLayer<TextLayer>(font);
				//registerLayer<Win32TestLayer>();
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationDescriptor applicationDescriptor{};
		applicationDescriptor.projectName = PROJECT_NAME;

		return new SandboxApplication(applicationDescriptor);
	}
}