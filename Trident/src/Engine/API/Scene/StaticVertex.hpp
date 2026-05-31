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

	struct StaticTriangle3D {
		uint32 aIndex;
		uint32 bIndex;
		uint32 cIndex;
		uint32 normalIndex;
		uint32 aUvIndex;
		uint32 bUvIndex;
		uint32 cUvIndex;
	};
}