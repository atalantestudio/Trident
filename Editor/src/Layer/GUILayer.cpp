#include "GUILayer.hpp"

namespace ProjectA {
	GUILayer::GUILayer() :
		Layer("GUILayer")
	{}

	void GUILayer::bind() {}

	void GUILayer::unbind() {}

	void GUILayer::update(const float32 deltaTime) {
		renderer.render();
	}

	void GUILayer::renderImGui() {}

	void GUILayer::onEvent(Event& event) {}
}