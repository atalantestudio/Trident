// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Layer/DebugConsoleLayer.hpp"

namespace ProjectB {
	void DebugConsoleLayer::registerAssets() {
		updateThemeColors(true);

		UIDefaultTheme::registerAssets();

		clearColor = 0x00000000;
	}

	void DebugConsoleLayer::render() {
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

	void DebugConsoleLayer::onKeyDownClient(KeyDownEvent& event) {
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

				textInputString = UTF8String();

				const Command command = parseCommand(log.text);

				processCommand(command);

				break;
			}
			default: {
				break;
			}
		}
	}

	void DebugConsoleLayer::ConsoleTextInput() {
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

	void DebugConsoleLayer::processCommand(const Command& command) {
		if (command.name == "loadmap") {
			// TODO(command/loadmap)
		}
		else {
			Log commandNotFound{};
			commandNotFound.text = Logger::format("[]: command not found", command.name);
			commandNotFound.backgroundColor = 0xffa198ff;
			commandNotFound.foregroundColor = 0x0a0a0aff;

			logs.emplace_back(commandNotFound);
		}
	}
}