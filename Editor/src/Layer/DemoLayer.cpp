#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer() :
		Layer("Demo")
	{}

	void DemoLayer::bind() {}

	void DemoLayer::unbind() {}

	void DemoLayer::update(float32 deltaTime) {
		renderer.render();
	}

	void DemoLayer::onEvent(Event& event) {}
}