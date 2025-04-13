#include "pch.hpp"

#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer(/*const TrueTypeFont& font*/) {
		//renderer.build(font.getSdfImage());
	}

	void DemoLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}