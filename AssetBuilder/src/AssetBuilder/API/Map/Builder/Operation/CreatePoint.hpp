// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "AssetBuilder/API/Map/Builder/MapBuilder.hpp"

namespace Trident {
	void MapBuilder::createPoint(vec3<float32> position) {
		facePositions[facePositionCount] = position;
		facePositionCount += 1;
	}
}