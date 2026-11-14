// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Hull/HullHeader.hpp"

namespace Trident {
	struct BoxHull2D {
		HullHeader2D header;
		vec2<float32> vertices[4];
	};
}