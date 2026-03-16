// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/ECS/Entity.hpp"

namespace Trident {
	struct EntityPair {
		Entity A;
		Entity B;
	};

	struct EntityContactPair2D {
		Entity A;
		Entity B;
		ContactManifold2D contactManifold;

		// TODO: Differentiate static and dynamic friction.
		float32 combinedFriction;

		// TODO: Store combined restitution.
		// float32 combinedRestitution;
	};

	inline bool operator<(EntityPair a, EntityPair b) {
		if (a.A < b.A) {
			return true;
		}

		if (a.A == b.A && a.B < b.B) {
			return true;
		}

		return false;
	}
}