// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/3D/ClipVertex3D.hpp"

namespace Trident {
	struct EdgeFeature3D {
		EdgeFeature3D() {}

		union {
			ClipVertex3D vertex;
			ClipVertex3D edge[2];
		};

		uint8 vertexCount;
	};
}