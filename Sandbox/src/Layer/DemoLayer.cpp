#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer(const TTFFile& ttfFile) :
		renderer()
	{
		//renderer.build(ttfFile);
	}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}