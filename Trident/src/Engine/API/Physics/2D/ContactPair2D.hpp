// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/Contact/ContactManifold.hpp"

namespace Trident {
	struct ContactPair2D {
		ContactManifold2D manifold;

		// TODO: Differentiate static and dynamic friction.
		float32 combinedFriction;

		// TODO: Store combined restitution.
		// float32 combinedRestitution;
	};
}