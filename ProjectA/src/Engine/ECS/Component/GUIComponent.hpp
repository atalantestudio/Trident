#pragma once

namespace ProjectA {
	// Simple quad for 2D GUIs.
	struct GUIComponent {
		// TODO: Replace with client-side texture transform matrix.
		vec2<float32> textureCoordinates[4];

		mat3x3<float32> transform;
		uint32 colorIndex = 0;
		uint32 textureIndex = 0;
	};
}