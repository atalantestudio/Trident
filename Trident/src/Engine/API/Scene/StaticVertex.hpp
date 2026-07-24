// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct StaticTriangle2D {
		uint32 aIndex;
		uint32 bIndex;
		uint32 cIndex;
		uint32 aUvIndex;
		uint32 bUvIndex;
		uint32 cUvIndex;
	};

	struct StaticVertex3D {
		uint32 vertexIndex;
		uint32 textureCoordinateIndex;
		svec2<float32> lightmapCoordinate;
	};

	struct StaticTriangle3D {
		StaticVertex3D vertices[3];
		uint32 normalIndex;
	};
}