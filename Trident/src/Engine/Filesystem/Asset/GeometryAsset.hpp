// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Edge.hpp"
#include "Engine/API/Geometry/Hull2DType.hpp"
#include "Engine/API/Geometry/Hull3DType.hpp"
#include "Engine/API/Geometry/Plane3D.hpp"
#include "Engine/Filesystem/Asset/Asset.hpp"
#include "Engine/Filesystem/Asset/GeometryAssetFlags.hpp"

namespace Trident {
	struct Geometry2DAsset : public Asset {
		uint32 getPositionCount() const {
			return static_cast<uint32>(vertexPositionComponents.count() / 2);
		}

		uint32 getTextureCoordinateCount() const {
			return static_cast<uint32>(vertexTextureCoordinateComponents.count() / 2);
		}

		GeometryAssetFlags flags;
		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexPositionIndices;

		Hull2DType hullType;
		float32 hullRadius;
		svec2<float32> hullCentroid;
		sequence<uint32> hullLineIndices;
	};

	struct Geometry3DAsset : public Asset {
		GeometryAssetFlags flags;

		// TODO(asset/geometry/3d): Replace with a Vertex3D sequence and an index sequence.
		sequence<svec3<float32>> positions;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<svec3<float32>> tangents;
		uint32 indexCount;
		sequence<uint32> positionIndices;
		sequence<uint32> normalIndices;
		sequence<uint32> textureCoordinateIndices;

		Hull3DType hullType;
		float32 hullRadius;
		svec3<float32> hullCentroid;
		sequence<Edge3D> hullEdges;
		sequence<Plane3D> hullFaces;
		sequence<uint16> hullFaceEdgeIndices;
		sequence<uint16> hullLineIndices;
	};
}