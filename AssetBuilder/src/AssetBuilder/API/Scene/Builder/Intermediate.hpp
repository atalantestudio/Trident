// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Geometry/Geometry3D.hpp"
#include "AssetBuilder/API/Transform.hpp"

namespace Trident {
	struct IntermediateGeometry {
		uint32 assetIdentifier;

		// TODO(asset-builder/scene): For static geometries, if their shared instance count is 1, they can be baked into the scene.
		/// Shared instance count across all associated material geometries.
		uint32 sharedInstanceCount;

		sequence<BuildVertex3D> vertices;
		sequence<uint32> indices;
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