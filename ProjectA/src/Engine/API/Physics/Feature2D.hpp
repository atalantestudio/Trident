#pragma once

namespace ProjectA {
	// A part of an object's geometry, in world-space.
	struct Feature2D {
		// NOTE: The vertex/edge union deletes the default constructor,
		//       and because of that structs that use Feature2D cannot be constructed.
		Feature2D() {}

		// The vertices that compose the feature.
		// NOTE: The mesh transform is already applied on the feature's vertices.
		union {
			vec2<float32> vertex;
			vec2<float32> edge[2];
		};

		union {
			uint32 vertexIndex;
			uint32 edgeIndices[2];
		};

		// The number of vertices used.
		flag count : 2;
	};
}