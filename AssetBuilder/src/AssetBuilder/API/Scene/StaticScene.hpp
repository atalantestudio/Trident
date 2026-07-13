// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/StaticVertex.hpp"

namespace Trident {
	struct BuildStaticMaterial3D {
		uint32 identifier;
		range<uint32, uint32> triangleRange;
	};

	struct StaticScene2D {
		sequence<StaticTriangle2D> triangles;
		sequence<svec2<float32>> vertices;
		sequence<svec2<float32>> textureCoordinates;
	};

	struct StaticScene3D {
		sequence<svec3<float32>> vertices;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<svec2<float32>> lightmapCoordinates;
		sequence<StaticTriangle3D> triangles;
		sequence<BuildStaticMaterial3D> materials;
		uint32 lightmapAtlasImageAssetIdentifier = max<uint32>();
	};
}