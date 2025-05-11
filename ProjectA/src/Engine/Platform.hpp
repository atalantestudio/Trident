#pragma once

namespace ProjectA {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			static const char* getWindowName();

			// Sets the window name.
			// This name is used as the title of application windows and error pop-ups.
			static void setWindowName(const char* windowName);

			// Returns the identifier of the current process.
			static uint32 getCurrentProcessId();

			// Shows an fatal error message box, then exits.
			static void fatalExit(uint32 code, const std::string& message);

			static void waitForInput();

		private:
			static const char* windowName;
	};
}