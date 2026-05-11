// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUIPropertyPanelLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();
			}

			void render() override {
				const Extent2D extent = getApplication()->getWindow()->getExtent();

				Container({
					.size = {static_cast<float32>(extent.width - 16_px), static_cast<float32>(extent.height - 16_px)},
					.padding = {8_px, 8_px},
					.layoutDirection = LayoutDirection::VERTICAL,
					.layoutSpacing = 8_px,
					.color = PRIMARY_COLOR,
				}) {
					if (Button("Light")) {
						verticalListScrollOffset += 10_px;
					}

					VerticalList({
						.size = {300_px, 30_px},
						.padding = {1_px, 1_px},
						.layoutDirection = LayoutDirection::VERTICAL,
						.layoutOffset = verticalListScrollOffset,
						.layoutSpacing = 1_px,
						.color = SECONDARY_COLOR,
						.debugName = "VList",
					}) {
						for (uint32 i = 0; i < 20; i += 1) {
							Container({
								.size = {EXPAND, FIT},
								.color = TERTIARY_COLOR,
							}) {
								const sequence<char8> text = Logger::format("Element []", i);

								Text(text, {
									.color = TEXT_COLOR,
									.font = fontRegular,
									.fontSize = 16_px,
								});
							}
						}
					}
				}
			}

		private:
			float32 verticalListScrollOffset = 0_px;
	};
}