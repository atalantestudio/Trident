#include "pch.hpp"

#include "TextLayer.hpp"

namespace ProjectA {
	TextLayer::TextLayer(const TrueTypeFont& font) :
		renderer(font)
	{
		renderer.setText("PP Radio Grotesk", 64);
	}

	void TextLayer::update(float32 deltaTime, const ref<CommandEncoder>& commandEncoder, uint32 nextImageIndex) {
		/*GUIWindow window;

		{
			GUIButton button;
			button.text = "ABC";
			button.padding = {0.0f, 0.0f, 0.0f, 0.0f};

			if (button.clicked()) {
				//
			}
		}*/

		renderer.render(commandEncoder, nextImageIndex);
	}
}