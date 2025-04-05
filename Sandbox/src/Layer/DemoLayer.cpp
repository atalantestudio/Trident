#include "pch.hpp"

#include "Engine/BuildProcess/SDFGlyphMapBuildProcess.hpp"
#include "Engine/Renderer/Font/TrueType/TrueTypeFont.hpp"

#include "DemoLayer.hpp"

namespace ProjectA {
	DemoLayer::DemoLayer() {
		//const TrueTypeFont font("assets/fonts/test.ttf");
		const TTFFile ttfFile = Filesystem::load<TTFFile>("assets/fonts/test.ttf");

		// TODO: Move into TrueTypeFont.
		const ref<Image> glyphMapImage = SDFGlyphMapBuildProcess::build(ttfFile);

		renderer.build(glyphMapImage);
	}

	void DemoLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}