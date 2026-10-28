// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Command/Command.hpp"

#include "UI/DefaultTheme.hpp"

namespace ProjectB {
	class ConsoleLayer : public UIDefaultTheme {
		private:
			struct Log {
				sequence<char8> text;
				Color backgroundColor;
				Color foregroundColor;
			};

		public:
			void registerAssets() override;

		public:
			void render() final override;

		protected:
			void onKeyDownClient(KeyDownEvent& event) final override;

		private:
			void ConsoleTextInput();

			void processCommand(const Command& command);

		private:
			float32 logListOffset = 0.0f;
			bool textInputFocused = false;
			UTF8String textInputString;
			float32 textInputLayoutOffset = 0.0f;
			std::vector<Log> logs;
	};
}