#include "ProjectA.hpp"
#include "Engine/entry_point.hpp"

#include "Layer/SandboxLayer.hpp"
#include "Random.hpp"

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication() {
			registerLayer<SandboxLayer>();

			Random::initialize();
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}