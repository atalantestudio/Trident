#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer() {}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}