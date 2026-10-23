// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUITextInputLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();

				updateThemeColors(false);
			}

			void render() override {
				ExampleTextInput();
			}

		private:
			void ExampleTextInput() {
				Container({
					.size = {512_px, FIT},
					.color = backgroundHoverColor,
				}) {
					TextInput({
						.width = EXPAND,
						.color = textColor,
						.text = &text,
						.font = fontRegular,
						.fontSize = 16_px,
					});
				}
			}

		private:
			UTF8String text;
	};
}