#pragma once

namespace ProjectA {
	// Support for generic one-time and/or non-graphical computations.
	class Process {
		public:
			// NOTE: Process computations are ran through the constructor.

			virtual ~Process() = default;
	};
}