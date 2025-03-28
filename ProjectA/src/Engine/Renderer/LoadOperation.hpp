#pragma once

namespace ProjectA {
	enum class LoadOperation : uint8 {
		// Preserve the contents within the render area.
		LOAD = 0,

		// Clear the previous contents within the render area.
		CLEAR = 1,

		// Don't preserve the previous contents within the render area;
		// mark them as undefined.
		UNDEFINED = 2,
	};
}