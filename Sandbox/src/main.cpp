#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/DemoLayer.hpp"
#include "Layer/StatisticsLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const ApplicationDescriptor& applicationDescriptor) :
				Application(applicationDescriptor)
			{
				registerLayer<DemoLayer>();
				registerLayer<StatisticsLayer>();
			}

			~SandboxApplication() {}
	};

	Application* ProjectA::createApplication() {
		ApplicationDescriptor applicationDescriptor{};
		applicationDescriptor.projectName = PROJECT_NAME;

		return new SandboxApplication(applicationDescriptor);
	}
}