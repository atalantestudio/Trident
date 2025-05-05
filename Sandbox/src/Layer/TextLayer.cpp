#include "pch.hpp"

#include "TextLayer.hpp"

namespace ProjectA {
	TextLayer::TextLayer(const TrueTypeFont& font) :
		renderer(font)
	{
		renderer.setText("PP Radio Grotesk", 64);
	}

	void TextLayer::render(float64 remainingFrameTime, const ref<CommandEncoder>& commandEncoder) const {
		renderer.render(commandEncoder);
	}
}