#include "TriangleDemoLayer.hpp"

namespace ProjectA {
	TriangleDemoLayer::TriangleDemoLayer() :
		Layer("TriangleDemo")
	{}

	void TriangleDemoLayer::bind() {}

	void TriangleDemoLayer::unbind() {}

	void TriangleDemoLayer::update(float32 deltaTime) {
		renderer.render();
	}

	void TriangleDemoLayer::renderImGui() {}

	void TriangleDemoLayer::onEvent(Event& event) {}
}