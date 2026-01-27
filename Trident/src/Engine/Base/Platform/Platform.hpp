#pragma once

#include "Engine/Base/Platform/PlatformLibrary.hpp"

namespace Trident {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			// `windowName` is the title of windows and error pop-ups belonging to the current application.
			static void initialize(const char8* windowName);

			// Returns the identifier of the current process.
			static uint32 getCurrentProcessId();

			static PlatformLibrary openLibrary(view<char8> libraryName);

			// Shows an fatal error message box, then exits.
			static void fatalExit(uint32 code, view<char8> message);

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