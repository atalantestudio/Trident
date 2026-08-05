// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Geometry/Hull3D.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct Geometry2DData {
		GeometryAssetFlags flags;
		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;
		Geometry2D geometry;
		Hull2D hull;
	};

	struct Geometry3DData {
		GeometryAssetFlags flags;
		sequence<svec3<float32>> positions;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<svec3<float32>> tangents;
		sequence<uint32> positionIndices;
		sequence<uint32> normalIndices;
		sequence<uint32> textureCoordinateIndices;

		sequence<uint16> hullLineIndices;
		sequence<Edge3D> hullEdges;
		sequence<uint16> hullFaceEdgeIndices;
		sequence<Plane3D> hullFaces;
		Hull3D hull;
	};
}