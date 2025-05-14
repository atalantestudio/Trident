#pragma once

namespace ProjectA {
	struct ClosestFeature {
		// The vertices that compose the closest feature.
		vec2<float32> vertices[2];

		// The number of used vertices.
		uint8 count : 2;
	};
}