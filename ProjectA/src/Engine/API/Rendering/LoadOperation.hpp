#pragma once

namespace ProjectA {
	enum class LoadOperation : uint8 {
		// Preserves the contents within the render area.
		LOAD = 0,

		// Clears the previous contents within the render area.
		CLEAR = 1,

		// Doesn't preserve the previous contents within the render area and marks them as undefined.
		UNDEFINED = 2,
	};
}