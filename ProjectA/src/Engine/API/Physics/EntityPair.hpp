#pragma once

#include "Engine/API/Physics/Contact/ContactManifold.hpp"
#include "Engine/ECS/Entity.hpp"

namespace ProjectA {
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
}