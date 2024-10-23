#include "ProjectA.hpp"
#include "ProjectA/entry_point.hpp"

#include "Layer/SandboxLayer.hpp"

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication() {
			pushLayer(new SandboxLayer());
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}