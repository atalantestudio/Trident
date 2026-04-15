// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Geometry/Hull2D.hpp"
#include "Engine/API/Geometry/Hull3D.hpp"

namespace Trident {
	struct Geometry2DData {
		std::vector<float32> vertexComponentBuffer;
		std::vector<uint32> indexBuffer;
		Geometry2D geometry;
		Hull2D hull;
	};

	struct Geometry3DData {
		sequence<float64> vertexPositionComponents;
		sequence<float64> vertexNormalComponents;
		sequence<float64> vertexTextureCoordinateComponents;
		sequence<float32> triangleTangentComponents;
		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;

		GeometryAssetFlags flags;
		sequence<uint16> hullLineIndices;
		sequence<Edge3D> hullEdges;
		sequence<Face3D> hullFaces;
		Hull3D hull;
	};
}