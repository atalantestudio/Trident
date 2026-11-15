// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct HullHeader2D {
		inline const vec2<float32>* getVertices() const {
			return pointer_cast<const vec2<float32>*>(pointer_cast<const uint8*>(this) + vertexOffset);
			// return (const vec2<float32>*) vertexOffset;
		}

		/// The local centroid.
		vec2<float32> centroid;

		/// The area in m².
		float32 area;

		/// The distance between the center and the farthest vertex.
		float32 radius;

		float32 mass;

		// TODO(physics/2d/hull/bounds)
		/// Axis-aligned box in local space.
		// AABB2D bounds;

		/// The inertia about the centroid.
		float32 inertia;

		uint32 vertexCount;

		/// Byte offset of the vertex array in clockwise order.
		uint64 vertexOffset;

		uint32 userIndex;
	};
}