// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "UI/DefaultTheme.hpp"

namespace ProjectB {
	// TODO(project-b/console/log-color)
	// Error:
	// - backgroundColor = 0xffa198ff
	// - foregroundColor = 0x0a0a0aff
	class DebugConsoleLayer : public UIDefaultTheme {
		private:
			struct Log {
				sequence<char8> text;
				Color backgroundColor;
				Color foregroundColor;
			};

		public:
			void registerAssets() override {
				updateThemeColors(true);

				UIDefaultTheme::registerAssets();

				clearColor = 0x00000000;
			}

		public:
			void render() final override {
				const Extent2D screenSize = getApplication()->getWindow()->getExtent();
				const Extent2D consoleSize = min({968 + 4, 768 + 4}, screenSize);

				Container({
					.size = {EXPAND, EXPAND},
					.padding = {8_px, 8_px, 8_px, 8_px},
				}) {
					Container({
						.size = {static_cast<float32>(consoleSize.x) - 4_px, static_cast<float32>(consoleSize.y) - 4_px},
						.padding = {1_px, 1_px, 1_px, 1_px},
						.layoutDirection = VERTICAL,
						.layoutSpacing = 1_px,
						.color = secondaryColor,
					}) {
						Container({
							.size = {EXPAND, FIT},
							.padding = {4_px, 4_px, 2_px, 2_px},
						}) {
							Text("Debug Console", {
								.color = textColor,
								.font = font,
								.fontSize = 14_px,
							});
						}

						VerticalList({
							.size = {EXPAND, EXPAND},
							.padding = {4_px, 4_px, 4_px, 4_px},
							.scrollOffset = &logListOffset,
							.color = primaryColor,
						}) {
							for (const Log& log : logs) {
								Container({
									.size = {EXPAND, FIT},
									.color = log.backgroundColor,
								}) {
									Text(log.text, {
										.color = log.foregroundColor,
										.font = font,
										.fontSize = 14_px,
									});
								}
							}
						}

						ConsoleTextInput();
					}
				}
			}

		protected:
			void onKeyDownClient(KeyDownEvent& event) final override {
				switch (event.getKeyCode()) {
					case KeyCode::ENTER: {
						if (!textInputFocused) {
							break;
						}

						const view<char8> text = textInputString;

						Log log{};
						log.text = sequence<char8>(text.count());
						log.backgroundColor = 0x00000000;
						log.foregroundColor = 0xa5a5a5ff;

						std::copy(text.begin(), text.end(), &log.text[0]);

						logs.emplace_back(log);

						ENGINE_DEBUG(log.text);

						textInputString = UTF8String();

						break;
					}
					default: {
						break;
					}
				}
			}

		private:
			void ConsoleTextInput() {
				Container({
					.size = {EXPAND, FIT},
					.padding = {4_px, 4_px, 4_px, 4_px},
					.color = primaryColor,
				}) {
					textInputFocused = IsNextFocused();

					TextInput({
						.width = EXPAND,
						.layoutOffset = &textInputLayoutOffset,
						.color = textColor,
						.text = &textInputString,
						.font = font,
						.fontSize = 14_px,
					});
				}
			}

		private:
			float32 logListOffset = 0.0f;
			bool textInputFocused = false;
			UTF8String textInputString;
			float32 textInputLayoutOffset = 0.0f;
			std::vector<Log> logs;
	};
}