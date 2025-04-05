#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"

#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer(const TTFFile& ttfFile) :
		renderer()
	{
		const ref<Image> glyphMapImage = SDFGlyphMapBuildProcess::build(ttfFile);

		renderer.build(glyphMapImage);
	}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}