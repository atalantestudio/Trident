// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Scene/StaticVertex.hpp"

namespace Trident {
	struct StaticScene2D {
		sequence<StaticTriangle2D> triangles;
		sequence<svec2<float32>> vertices;
		sequence<svec2<float32>> textureCoordinates;
	};

	struct StaticScene3D {
		sequence<StaticTriangle3D> triangles;
		sequence<svec3<float32>> vertices;
		sequence<svec3<float32>> normals;
		sequence<svec2<float32>> textureCoordinates;
		sequence<svec2<float32>> lightmapCoordinates;
		uint32 lightmapAtlasImageAssetIdentifier = max<uint32>();
	};
}