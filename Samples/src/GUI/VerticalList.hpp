// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class VerticalListSample : public GUIDefaultTheme {
		private:
			static constexpr Color COLOR_ARRAY[] = {
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

			static constexpr view<Color> COLORS = COLOR_ARRAY;

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
						.size = {200_px, 300_px},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.layoutDirection = VERTICAL,
						.color = secondaryColor,
					}) {
						VerticalList({
							.size = {200_px, EXPAND},
							.scrollOffset = &scrollOffset,
							.color = primaryColor,
						}) {
							uint32 colorIndex = 0;

							while (colorIndex < COLORS.count() * 10) {
								const Color& color = COLORS[colorIndex % 10];

								Container({
									.size = {EXPAND, FIT},
									.color = color,
								}) {
									const sequence<char8> text = Logger::format("[]. Lorem ipsum dolor sit amet, consectetur adipiscing elit.", colorIndex);

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