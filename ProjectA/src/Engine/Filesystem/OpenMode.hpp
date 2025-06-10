#pragma once

namespace ProjectA {
	// TODO: Use.
	enum class OpenMode : uint8 {
		READ = BIT(0),
		WRITE = BIT(1),
		AT_END = BIT(2),
		DISCARD = BIT(4),
	};

	AS_FLAG(OpenMode);
}