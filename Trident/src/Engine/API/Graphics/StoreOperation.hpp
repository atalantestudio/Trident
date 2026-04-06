// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class StoreOperation : uint8 {
		// Stores the contents generated during the render pass and within the render area.
		STORE = 0,

		// Marks the contents generated during the render pass and within the render area as undefined.
		UNDEFINED = 1,

		count,
	};
}