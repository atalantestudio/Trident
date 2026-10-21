// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "UI/DefaultTheme.hpp"

namespace ProjectB {
	// TODO(project-b/layer/debug-menu)
	class DebugMenuLayer : public UIDefaultTheme {
		public:
			void registerAssets() override {
				updateThemeColors(false);

				UIDefaultTheme::registerAssets();

				clearColor = 0x00000000;
			}

		public:
			void render() final override {
				Container({
					.size = {1280_px, 720_px},
					.color = 0x30303050,
					.debugName = "console-container",
				}) {
					ConsoleTextInput();
				}
			}

		private:
			void ConsoleTextInput() {
				TextInput({
					.width = EXPAND,
					.padding = {16_px, 16_px, 16_px, 16_px},
					.color = 0xffffffff,
					.text = &text,
					.font = font,
					.fontSize = 16_px,
					.debugName = "console-text-input",
				});
			}

		private:
			UTF8String text;
	};
}