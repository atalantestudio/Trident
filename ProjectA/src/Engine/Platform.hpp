#pragma once

namespace ProjectA {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			// Initializes the platform.
			// This name is used as the title of application windows and error pop-ups.
			static void initialize(const char8* windowName);

			// Returns the identifier of the current process.
			static uint32 getCurrentProcessId();

			// Shows an fatal error message box, then exits.
			static void fatalExit(uint32 code, const std::string& message);

			static void waitForInput();

		private:
			static const char8* windowName;
	};
}