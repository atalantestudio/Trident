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
				Container({
					.size = {FIT, FIT},
					.padding = {8_px, 8_px, 8_px, 8_px},
				}) {
					Container({
						.size = {FIT, FIT},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.color = secondaryColor,
					}) {
						Container({
							.size = {FIT, FIT},
							.color = primaryColor,
						}) {
							TextInput({
								.width = 128_px,
								.layoutOffset = &layoutOffset,
								.color = textColor,
								.text = &text,
								.font = fontRegular,
								.fontSize = 16_px,
							});
						}
					}
				}
			}

		private:
			float32 layoutOffset = 0_px;
			UTF8String text;
	};
}