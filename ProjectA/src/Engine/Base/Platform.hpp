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

			static Endianness getEndianness() {
				static const Endianness endianness = calculateEndianness();

				return endianness;
			}

		private:
			static Endianness calculateEndianness() {
				static constexpr uint16 test = 1;

				return static_cast<Endianness>(*pointer_cast<const uint8*>(&test));
			}

		private:
			static const char8* windowName;
	};
}