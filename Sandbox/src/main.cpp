#include "ProjectA.hpp"

class TestLayer : public ProjectA::Layer {
	public:
		TestLayer() : Layer("Test") {}

		void update() override {}
		void listen(ProjectA::Event& event) override {}
};

class SandboxApplication : public ProjectA::Application {
	public:
		SandboxApplication() {
			pushLayer(new TestLayer());
			pushOverlay(new ProjectA::ImGuiLayer());
		}

		~SandboxApplication() {}
};

ProjectA::Application* ProjectA::createApplication() {
	return new SandboxApplication();
}