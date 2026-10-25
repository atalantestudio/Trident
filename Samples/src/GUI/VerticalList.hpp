// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class VerticalListSample : public GUIDefaultTheme {
		private:
			static constexpr Color COLORS[] = {
				0x5d8aa8ff,
				0xf0f8ffff,
				0xe32636ff,
				0xefdecdff,
				0xe52b50ff,
				0xffbf00ff,
				0xff033eff,
				0x9966ccff,
				0xa4c639ff,
				0xcd9575ff,
				0x915c83ff,
				0xfaebd7ff,
				0x008000ff,
			};

			static constexpr view<Color> COLOR_VIEW = COLORS;

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
					Container({
						.size = {FIT, 300_px},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.color = secondaryColor,
					}) {
						VerticalList({
							.size = {FIT, EXPAND},
							.scrollOffset = &scrollOffset,
							.color = primaryColor,
						}) {
							uint32 colorIndex = 0;

							while (colorIndex < COLOR_VIEW.count()) {
								const Color& color = COLOR_VIEW[colorIndex];

								Container({
									.size = {200_px, 32_px},
									.color = color,
								}) {
									const sequence<char8> text = Logger::format("Element []", colorIndex);

									Text(text, {
										.color = textColor,
										.font = fontRegular,
										.fontSize = 16_px,
									});
								}

								colorIndex += 1;
							}
						}
					}
				}
			}

		private:
			float32 scrollOffset = 0_px;
	};
}