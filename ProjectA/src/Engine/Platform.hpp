#pragma once

namespace ProjectA {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			// The project name is used as the title of error windows.
			static void setProjectName(const std::string& projectName);

			// Returns the current time in seconds.
			static float64 getTime();

			// Shows an error message box, then exits.
			static void fatalExit(uint32 code, const std::string& message);

		private:
			static std::string projectName;
	};
}