// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct HullHeader2D {
		/// The local centroid.
		vec2<float32> centroid;

		/// The area in m².
		float32 area;

		/// The distance between the center and the farthest vertex.
		float32 radius;

		// TODO(physics/2d/hull/bounds)
		/// Axis-aligned box in local space.
		// AABB2D bounds;

		/// The inertia about the centroid.
		float32 inertia;

		uint32 vertexCount;

		/// Byte offset of the vertex array in clockwise order.
		uint32 vertexOffset;

		uint32 userIndex;
	};
}