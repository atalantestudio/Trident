#pragma once

namespace Trident {
	class HighResolutionTimer {
		public:
			HighResolutionTimer();

			// Returns the time since the creation of the current process, in seconds.
			float64 getCurrentTime() const;

		private:
			sint64 startTime;
			sint64 timeFrequency;
	};
}