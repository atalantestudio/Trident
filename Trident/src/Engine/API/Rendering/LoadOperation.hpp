// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class LoadOperation : uint8 {
		// Preserves the contents within the render area.
		LOAD = 0,

		// Clears the previous contents within the render area.
		CLEAR = 1,

		// Marks the previous contents within the render area as undefined.
		UNDEFINED = 2,

		count,
	};
}