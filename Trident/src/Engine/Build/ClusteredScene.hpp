// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Material.hpp"
#include "Engine/API/Material/MaterialReference.hpp"
#include "Engine/Build/Cluster.hpp"
#include "Engine/Build/ClusteredGeometry.hpp"
#include "Engine/Build/ClusteredInstance.hpp"

namespace Trident {
	struct ClusteredScene {
		sequence<Cluster> clusters;
		sequence<ClusteredGeometry> geometries;
		sequence<ClusteredInstance> instances;

		sequence<float32> vertexPositionComponents;
		sequence<float32> vertexNormalComponents;
		sequence<float32> vertexTextureCoordinateComponents;

		sequence<uint32> vertexPositionIndices;
		sequence<uint32> vertexNormalIndices;
		sequence<uint32> vertexTextureCoordinateIndices;

		sequence<Material> materials;
		sequence<MaterialReference> materialReferences;
	};
}