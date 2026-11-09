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
	};

	struct RigidBody2D {
		RigidBodyType type;
		RigidBodyFlags2D flags;

		// TODO(physics/2d): Use delta position.
		vec2<float32> position;

		/// Position of the centroid in local space.
		vec2<float32> centroid;

		float32 rotation;

		float32 inverseMass;

		/// Inverse inertia about the center of mass.
		float32 inverseInertia;

		vec2<float32> linearVelocity;
		float32 angularVelocity;

		vec2<float32> force;

		// TODO(physics/2d/torque): Implement torque.
		float32 torque;
	};
}