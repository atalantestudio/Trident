// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUIPropertyPanelLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();

				updateThemeColors(false);
			}

			void render() override {
				Container({
					.padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
				}) {
					VerticalList({
						.size = {300_px, 200_px},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.scrollOffset = &scrollOffset,
						.layoutSpacing = 1_px,
						.color = secondaryColor,
						.debugName = "VList",
					}) {
						for (uint32 i = 0; i < 20; i += 1) {
							Container({
								.size = {EXPAND, FIT},
								.color = secondaryColor,
							}) {
								const sequence<char8> text = Logger::format("Element []", i);

								Text(text, {
									.color = textColor,
									.font = fontRegular,
									.fontSize = 16_px,
								});
							}
						}
					}
				}
			}

		private:
			float32 scrollOffset = 0_px;
	};
}