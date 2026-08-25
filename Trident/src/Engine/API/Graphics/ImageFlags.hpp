// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum ImageFlags : uint8 {
		/// Specifies that the image can be used to create image views of type CUBE.
		CUBE_COMPATIBLE = BIT(0),
	};

	AS_FLAG(ImageFlags);
}