#include "ProjectA.hpp"

class SandboxApplication : public ProjectA::Application {};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}