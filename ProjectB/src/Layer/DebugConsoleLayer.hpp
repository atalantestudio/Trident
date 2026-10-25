// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "UI/DefaultTheme.hpp"

namespace ProjectB {
	class DebugConsoleLayer : public UIDefaultTheme {
		public:
			void registerAssets() override {
				updateThemeColors(false);

				UIDefaultTheme::registerAssets();

				clearColor = 0x00000000;
			}

		public:
			void render() final override {
				const Extent2D consoleSize = min({968 + 4, 768 + 4}, getApplication()->getWindow()->getExtent());

				Container({
					.size = {static_cast<float32>(consoleSize.x) - 4_px, static_cast<float32>(consoleSize.y) - 4_px},
					.padding = {2_px, 2_px, 2_px, 2_px},
					.layoutDirection = VERTICAL,
					.layoutSpacing = 2_px,
					.color = 0x30303050,
					.debugName = "console-container",
				}) {
					Container({
						.size = {EXPAND, EXPAND},
					});

					ConsoleTextInput();
				}
			}

		private:
			void ConsoleTextInput() {
				Container({
					.size = {EXPAND, FIT},
					.padding = {1_px, 1_px, 1_px, 1_px},
					.color = 0x3a3a3aff,
				}) {
					Container({
						.size = {EXPAND, FIT},
						.color = 0x1a1a1aff,
					}) {
						TextInput({
							.width = EXPAND,
							.layoutOffset = &layoutOffset,
							.color = 0xffffffff,
							.text = &text,
							.font = font,
							.fontSize = 16_px,
							.debugName = "console-text-input",
						});
					}
				}
			}

		private:
			UTF8String text;
			float32 layoutOffset;
	};
}