// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	// TODO(geometry/index): Rename to Vertex3D.
	struct GeometryIndex3D {
		uint32 positionIndex;
		uint32 normalIndex;
		uint32 textureCoordinateIndex;
	};
}