#pragma once

namespace ProjectA {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			static const char* getWindowName();

			// Used as the title of application windows and error pop-ups.
			static void setWindowName(const char* windowName);

			static uint32 getProcessId();

			// Returns the current time in seconds.
			static float64 getTime();

			// Shows an error message box, then exits.
			static void fatalExit(uint32 code, const std::string& message);

			static void waitInput() {
				std::cin.get();
			}

		private:
			static const char* windowName;
	};
}