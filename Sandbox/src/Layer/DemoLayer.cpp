#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer(const TrueTypeFont& font) :
		renderer(font)
	{
		renderer.setText("PP Radio Grotesk.");
	}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}