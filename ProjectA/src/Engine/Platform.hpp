#pragma once

namespace ProjectA {
	// Platform-agnostic helper functions.
	class Platform {
		public:
			// Returns the current time in seconds.
			static float64 getTime();
	};
}