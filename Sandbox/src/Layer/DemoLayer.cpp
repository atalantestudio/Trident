#include "pch.hpp"

#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer(/*const TrueTypeFont& font*/) {
		//renderer.build(font.getSdfImage());
	}

	void DemoLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) {
		renderer.render(commandEncoder);
	}
}