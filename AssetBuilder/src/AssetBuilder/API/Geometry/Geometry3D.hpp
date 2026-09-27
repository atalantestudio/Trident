// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct BuildVertex3D {
		alignas(16) svec3<float32> position;
		alignas(16) svec3<float32> normal;
		vec4<float32> tangent;
		svec2<float32> textureCoordinate;
	};

	struct BuildGeometry3D {
		GeometryAssetFlags flags;
		sequence<BuildVertex3D> vertices;
		sequence<uint32> indices;

		Hull3D hull;
		sequence<svec3<float32>> hullVertices;
		sequence<Edge3D> hullEdges;
		sequence<uint16> hullFaceEdgeIndices;
		sequence<Plane3D> hullFaces;
		sequence<uint16> hullLineIndices;
	};

	struct BuildStaticGeometry3D {
		sequence<BuildVertex3D> vertices;
		sequence<uint32> indices;
		sequence<uint16> edgeOriginVertexIndices;
		vec3<float32> normal;
		svec2<float32> scale;
	};
}