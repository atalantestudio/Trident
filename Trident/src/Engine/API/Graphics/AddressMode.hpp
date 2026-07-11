// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	/// Specifies wrapping operations to use when sampling an image with out-of-bounds coordinates.
	enum class AddressMode : uint8 {
		/// Tiles the texture at every coordinate integer junction.
		REPEAT,

		/// Flips the texture at every coordinate integer junction.
		REPEAT_MIRRORED,

		/// Sets out-of-bounds coordinates to the edge's color.
		CLAMP_TO_EDGE,

		/// Sets out-of-bounds coordinates to the border's color.
		CLAMP_TO_BORDER,

		count,
	};
}