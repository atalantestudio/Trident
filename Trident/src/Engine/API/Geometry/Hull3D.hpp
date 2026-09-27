// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Geometry/Plane3D.hpp"
#include "Engine/API/Geometry/Vertex.hpp"

namespace Trident {
	/// A convex hull wrapping a 3D geometry.
	// TODO(physics/runtime-hull-creation): Some hulls may need to be created at runtime (e.g. the player collision hull).
	struct Hull3D {
		inline float32 getPenetrationBias() const {
			// TODO(physics/3d/hull-penetration): Return different biases for each hull type (sphere, capsule, polygon).
			return 0.2f;
		}

		inline vec3<float32> support(vec3<float32> d) const {
			float32 maxProjection = min<float32>();
			uint32 maxProjectionVertexIndex = max<uint32>();
			uint32 vertexIndex = 0;

			while (vertexIndex < vertices.count()) {
				const float32 projection = dot(vec3<float32>(vertices[vertexIndex]), d);

				if (projection > maxProjection) {
					maxProjection = projection;
					maxProjectionVertexIndex = vertexIndex;
				}

				vertexIndex += 1;
			}

			return vec3<float32>(vertices[maxProjectionVertexIndex]);
		}

		/// The mean position of all vertices.
		/// It coincides with the center of mass in uniform mass distribution scenarios.
		svec3<float32> centroid;

		/// The distance between the centroid and the farthest vertex.
		float32 radius;

		view<svec3<float32>> vertices;

		/// The edges composing the hull, arranged in a sequence of half-edges.
		/// Each even edge is followed by its twin edge.
		view<Edge3D> edges;

		/// The faces composing the hull.
		view<Plane3D> faces;

		/// The edge indices per face.
		view<uint16> faceEdgeIndices;

		/// Line indices used for wireframe rendering.
		range<uint32, uint16> lineIndexRange;

		/// The type of the hull, used in physics optimizations.
		Hull3DType type;
	};
}