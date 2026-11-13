// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"

namespace Trident {
	struct ContactPair2D {
		float32 friction;
		ContactManifold2D manifold;
	};
}