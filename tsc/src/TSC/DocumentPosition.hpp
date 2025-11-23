#pragma once

namespace ProjectA {
	struct DocumentPosition {
		uint32 lineIndex;

		// Offset relative to the start of the line.
		uint32 characterOffset;
	};

	inline TextBuffer& operator<<(TextBuffer& buffer, DocumentPosition position) {
		return buffer << position.lineIndex + 1 << ':' << position.characterOffset + 1;
	}
}