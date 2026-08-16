// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(graphics/clear-value): Restore union.
	struct ClearValue {
		/// The color image clear values to use when clearing a color image or attachment.
		union {
			// TODO(lyah/scalar): Replace with a vector.
			float32 color[4];

			sint32 colorSigned[4];
			uint32 colorUnsigned[4];
		};

		/// The depth clear value to use when clearing a depth image or attachment.
		float32 depth = 0.0f;

		/// The stencil clear value to use when clearing a stencil image or attachment.
		uint32 stencil = 0;
	};
}