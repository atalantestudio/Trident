// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/EdgeFeature3D.hpp"

namespace Trident {
	struct FaceFeature3D {
		EdgeFeature3D edges[8];
		uint8 edgeCount;
	};
}