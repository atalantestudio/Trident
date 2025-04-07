#include "pch.hpp"

#include "TextLayer.hpp"

namespace ProjectA {
	TextLayer::TextLayer(const TrueTypeFont& font) :
		renderer(font)
	{
		renderer.setText("PP Radio Grotesk.");
	}

	void TextLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		renderer.render(commandEncoder, nextImageIndex);
	}
}