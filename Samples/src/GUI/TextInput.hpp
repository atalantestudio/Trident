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
				Color color = secondaryColor;

				if (IsNextFocused()) {
					color = borderFocusColor;
				}

				TextInput({
					.width = EXPAND,
					.padding = {1_px, 1_px, 1_px, 1_px},
					.color = color,
					.text = &text,
				}) {
					Color color = primaryColor;

					if (IsHovered()) {
						color = backgroundHoverColor;
					}

					Container({
						.size = {EXPAND, FIT},
						.padding = {4_px, 4_px, 4_px, 4_px},
						.color = color,
					}) {
						Text(text, {
							.color = textColor,
							.font = fontRegular,
							.fontSize = 16_px,
						});
					}
				}
			}

		private:
			UTF8String text;
	};
}