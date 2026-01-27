// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/ECS/Entity.hpp"

namespace Trident {
	typedef uint64 EntityPairKey;

	struct EntityPair {
		EntityPairKey asKey() const {
			return (static_cast<EntityPairKey>(A) << 32) | B;
		}

		void parseKey(EntityPairKey key) {
			A = static_cast<Entity>(key >> 32);
			B = static_cast<Entity>(key);
		}

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
}