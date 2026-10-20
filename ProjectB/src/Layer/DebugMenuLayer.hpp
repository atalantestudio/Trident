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
				}) {
					ConsoleTextInput();
				}
			}

		private:
			void ConsoleTextInput() {
				Color color = secondaryColor;

				if (IsNextFocused()) {
					color = borderFocusColor;
				}

				TextInput({
					.size = {EXPAND, FIT},
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
						const view<char8> textView(&text[0], text.size());

						Text(textView, {
							.color = textColor,
							.font = fontRegular,
							.fontSize = 16_px,
						});
					}
				}
			}

		private:
			std::string text;
	};
}