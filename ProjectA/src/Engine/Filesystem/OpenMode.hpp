#pragma once

namespace ProjectA {
	// TODO: Use.
	enum class OpenMode : uint8 {
		// Opens a file for reading.
		READ = BIT(0),

		// Opens a file for writing.
		WRITE = BIT(1),

		// Moves the cursor at the end of the file when opening.
		AT_END = BIT(2),

		// Discards the contents of the file when opening.
		DISCARD = BIT(4),
	};

	AS_FLAG(OpenMode);
}