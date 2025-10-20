#pragma once

namespace ProjectA {
	struct Position {
		uint32 lineIndex;

		// Offset relative to the start of the line.
		uint32 characterOffset;
	};
}