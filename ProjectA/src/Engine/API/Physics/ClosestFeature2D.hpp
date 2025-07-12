#pragma once

namespace ProjectA {
	// The vertex or edge on an object that is the closest to another object.
	struct ClosestFeature2D {
		// The vertex/edge union deletes the default constructor,
		// and because of that structs that use ClosestFeature2D (like GJK2DResult) cannot be constructed.
		ClosestFeature2D() {}

		// TODO: Store the index of the vertices instead of the transformed vertices.
		//       In CCD, the non-transformed vertex is needed to create the separation function.
		// The vertices that compose the feature.
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