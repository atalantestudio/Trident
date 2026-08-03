// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Scene/Scene3D.hpp"

#include "Engine/API/Geometry/Index.hpp"
#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct IntermediateGeometry {
		uint32 assetIdentifier;

		// TODO(asset-builder/scene): For static geometries, if their shared instance count is 1, they can be baked into the scene.
		/// Shared instance count across all associated material geometries.
		uint32 sharedInstanceCount;

		sequence<GeometryIndex3D> vertices;
		sequence<svec3<float32>> positions;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<svec2<float32>> transformedTextureCoordinates;
	};

	struct IntermediateMaterialGeometry {
		std::vector<BuildTransform3D> instances;
		uint32 geometryIndex;
	};

	struct IntermediateMaterial {
		uint32 assetIdentifier;
		std::vector<IntermediateMaterialGeometry> geometries;
	};

	struct IntermediateMaterialShader {
		uint32 assetIdentifier;
		std::vector<IntermediateMaterial> materials;
	};
}