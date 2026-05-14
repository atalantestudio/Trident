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
				TextInput();
			}

		private:
			void TextInput() {
				Container({
					.componentType = GUIComponentType::TEXT_INPUT,
					.size = {256_px, 40_px},
					.color = IsNextFocused() ? 0xde1818ff : 0xff9800ff,
					.focusable = true,
				});
			}
	};
}