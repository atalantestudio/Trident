#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer() :
		Layer("Demo"),
		renderer()
	{}

	void DemoLayer::bind() {}

	void DemoLayer::unbind() {}

	void DemoLayer::update(const float32 deltaTime) {
		renderer.render();
	}

	void DemoLayer::renderImGui() {}

	void DemoLayer::onEvent(Event& event) {}
}