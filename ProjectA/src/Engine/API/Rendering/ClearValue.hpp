#pragma once

namespace ProjectA {
	// TODO: Restore union.
	struct ClearValue {
		// TODO: Replace by vec4<float32>.
		// The color image clear values to use when clearing a color image or attachment.
		float32 color[4];

		// The depth clear value to use when clearing a depth image or attachment.
		float32 depth = 0.0f;

		// The stencil clear value to use when clearing a stencil image or attachment.
		uint32 stencil = 0;
	};
}