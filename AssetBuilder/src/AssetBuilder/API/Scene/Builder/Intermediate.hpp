// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/ShaderModuleAsset.hpp"

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Transform.hpp"

namespace Trident {
	struct IntermediateGeometry {
		uint32 assetIdentifier;

		// TODO(asset-builder/scene): For static props, if their shared instance count is 1 they can be baked into the scene.
		/// Shared instance count across all material geometries.
		uint32 sharedInstanceCount;

		bool staticUsage = false;

		sequence<BuildVertex3D> vertices;
		sequence<uint32> indices;

		/// Index obtained after filtering the intermediate geometries.
		uint32 filteredIndex = max<uint32>();
	};

	struct IntermediateMaterialGeometry {
		uint32 geometryIndex;
		std::vector<BuildTransform3D> instances;
	};

	struct IntermediateMaterial {
		uint32 assetIdentifier;
		std::vector<IntermediateMaterialGeometry> geometries;

		/// Index into IntermediateMaterial::geometries.
		uint32 bakedGeometryIndex;
	};

	struct IntermediateMaterialShader {
		ShaderModuleAsset asset;
		std::vector<IntermediateMaterial> materials;
	};
}