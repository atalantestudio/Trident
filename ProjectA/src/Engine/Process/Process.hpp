#pragma once

namespace ProjectA {
	// Support for generic one-time and/or non-graphical computations.
	class Process {
		public:
			// NOTE: Process computations are ran through the constructor.
			// TODO: Make computations run inside sint32 main().

			virtual ~Process() = default;
	};
}