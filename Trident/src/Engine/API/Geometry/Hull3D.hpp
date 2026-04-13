// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull3DType.hpp"

namespace Trident {
	/// A convex hull wrapping a 3D geometry.
	struct Hull3D {
		// TODO: Implement.
		// explicit Hull3D(ref<GeometryAsset> geometryAsset, view<svec3<float32>> vertexPositions, uint32 lineIndexOffset);

		// TODO(physics/runtime-hull-creation): Some hulls may need to be created at runtime (e.g. the player collision hull).

		/// The mean position of all vertices.
		/// It coincides with the center of mass in uniform mass distribution scenarios.
		svec3<float32> centroid;

		/// The distance between the centroid and the farthest vertex.
		float32 radius;

		view<svec3<float32>> vertexPositions;

		view<HalfEdge> edges;

		/// NOTE: Cannot contain more than 65535 normals.
		view<svec3<float32>> faceNormals;

		// TODO: Use 16-bit indices.
		/// Line indices used for wireframe rendering.
		range<uint32, uint16> lineIndexRange;

		/// The type of the hull, used in physics optimizations.
		Hull3DType type;
	};
}