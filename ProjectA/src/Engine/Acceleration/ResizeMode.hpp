#pragma once

namespace ProjectA {
	enum class ResizeMode : uint32 {
		// Update the size to the double of the original.
		DOUBLE = 0,
		// Update the size to the square of the original.
		SQUARE = 1,
	};
}