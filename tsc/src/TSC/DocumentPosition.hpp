#pragma once

namespace ProjectA {
	struct DocumentPosition {
		uint32 lineIndex;

		// Offset relative to the start of the line.
		uint32 characterOffset;
	};
}