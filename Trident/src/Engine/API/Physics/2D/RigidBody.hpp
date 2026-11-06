// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBodyFlags.hpp"
#include "Engine/API/Physics/RigidBodyType.hpp"

namespace Trident {
	struct RigidBodyDescriptor2D {
		RigidBodyType type;
		RigidBodyFlags2D flags;

		/// Position of the center of mass in world-space.
		vec2<float32> position;

		float32 rotation;

		/// Initial linear velocity.
		vec2<float32> linearVelocity;

		/// Initial angular velocity.
		float32 angularVelocity;

		float32 mass;
		float32 inertia;
	};

	struct RigidBody2D {
		RigidBodyType type;

		// TODO(physics/2d): Use delta position.
		vec2<float32> position;

		float32 rotation;

		vec2<float32> linearVelocity;
		float32 angularVelocity;

		/// Position of the centroid in local space.
		vec2<float32> centroid;

		RigidBodyFlags2D flags;

		vec2<float32> force;
		float32 torque;
		float32 inverseMass;

		/// Inverse inertia about the center of mass.
		float32 inverseInertia;
	};
}