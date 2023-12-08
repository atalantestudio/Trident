#include "ProjectA.hpp"

class TestLayer : public ProjectA::Layer {
	public:
		TestLayer() : Layer("Test") {}

		void update() override {
			LOG_CORE_INFO("From TestLayer: update");
		}

		void listen(ProjectA::Event& event) override {
			LOG_CORE_TRACE("From TestLayer: {0}", event);
		}
};

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication() {
			pushLayer(new TestLayer());
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}