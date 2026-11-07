// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct CircleShape2D {
		// In local space.
		vec2<float32> centroid;

		float32 radius;
	};
}