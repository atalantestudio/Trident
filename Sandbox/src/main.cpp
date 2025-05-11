#include "ProjectA.hpp"

#include "Engine/entry_point.hpp"

#include "Layer/GUI/StatisticsLayer.hpp"
#include "Layer/RendererLayer.hpp"

namespace ProjectA {
	class SandboxApplication : public Application {
		public:
			SandboxApplication(const ApplicationDescriptor& applicationDescriptor) :
				Application(applicationDescriptor)
			{
				registerLayer<RendererLayer>();
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