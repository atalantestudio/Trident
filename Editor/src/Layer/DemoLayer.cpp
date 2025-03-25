#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer() :
		Layer("Demo")
	{}

	void DemoLayer::bind() {}

	void DemoLayer::unbind() {}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}