// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/2D/RigidBodyFlags.hpp"
#include "Engine/API/Physics/RigidBodyType.hpp"

namespace Trident {
	struct RigidBodyDescriptor2D {
		RigidBodyType type;
		RigidBodyFlags2D flags;

		/// Initial position of the center of mass in world-space.
		vec2<float32> position;

		/// Initial rotation.
		float32 rotation;

		/// Initial linear velocity.
		vec2<float32> linearVelocity;

		/// Initial angular velocity.
		float32 angularVelocity;
	};

	struct RigidBody2D {
		// TODO(physics/2d): Debug-only.
		inline mat3x3<float32> getWorld() const {
			return (
				translation<mat3x3<float32>>(-centroid) *
				scaling<mat3x3<float32>>(scale) *
				rotationAngle<mat3x3<float32>>(rotation) *
				translation<mat3x3<float32>>(position)
			);
		}

		RigidBodyType type;
		RigidBodyFlags2D flags;

		// TODO(physics/2d): Use delta position and rotation.
		// Transform2D transform;
		vec2<float32> position;

		// TODO(physics/2d): Debug only.
		float32 rotation;

		// TODO(wip)
		vec2<float32> scale;

		/// Position of the centroid in local space.
		vec2<float32> centroid;

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