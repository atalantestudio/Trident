// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::removeFace(uint32 faceIndex) {
		DEBUG_ASSERT(faceIndex < staticGeometryFaceCount);

		staticGeometryFaceCount -= 1;
		faces[faceIndex] = faces[staticGeometryFaceCount];
	}
}