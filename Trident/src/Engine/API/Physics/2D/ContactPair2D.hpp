// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/ECS/Entity.hpp"

namespace Trident {
	struct ContactPair2D {
		Entity A;
		Entity B;
		float32 friction;
		ContactManifold2D manifold;
	};
}