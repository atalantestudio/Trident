// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Plane3D.hpp"
#include "Engine/API/Geometry/Vertex.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct Geometry3DAsset : public Asset {
		GeometryAssetFlags flags;
		sequence<Vertex3D> vertices;

		// TODO(asset/geometry/3d): Don't store in the release asset.
		/// High resolution normals for lightmap generation.
		sequence<vec3<float32>> vertexNormals;

		sequence<uint32> indices;
		Hull3DType hullType;
		float32 hullRadius;
		vec3<float32> hullCentroid;
		sequence<vec3<float32>> hullVertices;
		sequence<Edge3D> hullEdges;
		sequence<Plane3D> hullFaces;
		sequence<uint16> hullFaceEdgeIndices;
		sequence<uint16> hullLineIndices;
	};
}