#pragma once

namespace ProjectA {
	struct DocumentPosition {
		uint32 lineIndex;

		// Offset relative to the start of the line.
		uint32 characterOffset;
	};

	// TODO: Use std::ostream or std::ostringstream?
	inline std::ostream& operator <<(std::ostream& stream, DocumentPosition position) {
		stream << position.lineIndex + 1 << ":" << position.characterOffset + 1;

		return stream;
	}
}