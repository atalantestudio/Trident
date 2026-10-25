// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUISliderWidgetLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();

				updateThemeColors(darkTheme);
			}

			void render() override {
				Container({
					.size = {256_px, FIT},
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
				}) {
					if (Button(darkTheme ? "Dark" : "Light")) {
						darkTheme = !darkTheme;

						updateThemeColors(darkTheme);
					}

					Slider(value);
				}
			}

		private:
			void Slider(float32& value) {
				Container({
					.size = {EXPAND, FIT},
					.layoutDirection = LayoutDirection::VERTICAL,
				}) {
					Text("Label", {
						.color = secondaryColor,
						.font = fontRegular,
						.fontSize = 14_px,
					});

					const sequence<char8> text = Logger::format("[]", value);

					Color color = secondaryColor;

					if (IsNextFocused()) {
						color = borderFocusColor;
					}

					Container({
						.size = {EXPAND, FIT},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.color = color,
						.focusable = true,
					}) {
						Container({
							.size = {EXPAND, FIT},
							.padding = {8_px, 8_px, 2_px, 2_px},
							.color = primaryColor,
						}) {
							Text(text, {
								.color = textColor,
								.font = fontRegular,
								.fontSize = 16_px,
							});
						}
					}
				}
			}

		private:
			bool darkTheme = false;
			float32 value = 0.0f;
	};
}