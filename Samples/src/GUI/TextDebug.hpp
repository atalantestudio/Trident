// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Theme/DefaultTheme.hpp"

namespace Trident {
	class GUITextDebugLayer : public GUIDefaultTheme {
		public:
			void registerAssets() override {
				GUIDefaultTheme::registerAssets();

				updateThemeColors(darkTheme);
			}

			void render() override {
				Container({
					.size = {EXPAND, FIT},
					// .padding = {8_px, 8_px, 8_px, 8_px},
					.layoutDirection = LayoutDirection::HORIZONTAL,
				}) {
					Container({
						.size = {EXPAND, FIT},
						.layoutDirection = LayoutDirection::VERTICAL,
					}) {
						Samples();
					}

					Container({
						.size = {256_px, FIT},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.color = secondaryColor,
					}) {
						Container({
							.size = {EXPAND, FIT},
							.padding = {8_px, 8_px, 8_px, 8_px},
							.layoutDirection = LayoutDirection::VERTICAL,
							.layoutSpacing = 8_px,
							.color = primaryColor,
						}) {
							Controls();
						}
					}
				}
			}

		private:
			void Samples() {
				const float32 fontSize = 12_px + fontSizeMultiplier * 20.0f;
				const sequence<char8> text = Logger::format("Sample text ([]px)", fontSize);

				Text("Sample text A", {
					.color = textColor,
					.font = fontRegular,
					.fontSize = fontSize,
				});

				Text("Sample text B", {
					.color = textColor,
					.font = fontRegular,
					.fontSize = fontSize + 1.6_px,
				});

				Text("Sample text C", {
					.color = textColor,
					.font = fontRegular,
					.fontSize = fontSize,
				});
			}

			void Controls() {
				if (Button(darkTheme ? "Dark" : "Light")) {
					darkTheme = !darkTheme;

					updateThemeColors(darkTheme);
				}

				RangeControlFormField("Font Size Multiplier", fontSizeMultiplier, draggingFSM, previousOffsetFSM);
				RangeControlFormField("Hinting X", hintingX, draggingHX, previousOffsetHX);
				RangeControlFormField("Hinting Y", hintingY, draggingHY, previousOffsetHY);
			}

			void RangeControlFormField(view<char8> label, float32& value, bool& dragging, float32& previousOffset) {
				const sequence<char8> formattedValue = Logger::format("[]%", value);

				Container({
					.size = {EXPAND, FIT},
					.layoutDirection = LayoutDirection::VERTICAL,
				}) {
					Container({
						.size = {EXPAND, FIT},
						.layoutDirection = LayoutDirection::HORIZONTAL,
					}) {
						Text(label, {
							.color = secondaryColor,
							.font = fontRegular,
							.fontSize = 14_px,
						});

						Container({
							.size = {EXPAND, 0_px},
						});

						Text(formattedValue, {
							.color = secondaryColor,
							.font = fontRegular,
							.fontSize = 14_px,
						});
					}

					RangeControl(dragging, previousOffset, value);
				}
			}

		private:
			bool darkTheme = false;

			float32 fontSizeMultiplier = 0.5f;

			bool draggingFSM = false;
			bool draggingHX = false;
			bool draggingHY = false;

			float32 previousOffsetFSM = 0_px;
			float32 previousOffsetHX = 0_px;
			float32 previousOffsetHY = 0_px;
	};
}