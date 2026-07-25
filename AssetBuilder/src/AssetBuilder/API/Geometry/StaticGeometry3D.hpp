// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Transform.hpp"

#include "Engine/Filesystem/Asset/GeometryAsset.hpp"

namespace Trident {
	struct StaticGeometry3D {
		sequence<float32> vertexComponents;
		sequence<float32> vertexTextureCoordinateComponents;
		sequence<uint32> vertexIndices;
		sequence<uint32> vertexTextureCoordinateIndices;
		sequence<uint16> edgeOriginVertexIndices;
		vec3<float32> normal;
		svec2<float32> scale;
	};

	struct StaticGeometryDescriptor {
		ref<GeometryAsset> asset;
		EulerTransform transform;
		uint32 materialIndex;

		/// Artificial scale multiplier in case the geometry is too small.
		svec2<float32> lightmapScale = {1.0f, 1.0f};

		svec2<float32> luxelScale;
	};
}