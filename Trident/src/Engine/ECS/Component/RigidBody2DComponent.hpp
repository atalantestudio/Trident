// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct RigidBody2DComponent {
		void updateWorld(vec2<float32> centroid) {
			world = (
				translation<mat3x3<float32>>(-centroid) *
				scaling<mat3x3<float32>>(scale) *
				rotationAngle<mat3x3<float32>>(rotation) *
				translation<mat3x3<float32>>(position)
			);
		}

		/// Applies a linear force to the body.
		inline void applyLinearForce(vec2<float32> F) {
			force += F;
		}

		/// Applies linear and angular impulses to the body.
		inline void applyImpulse(
			/// The impulse vector.
			vec2<float32> j,

			/// The vector from the body's position to the contact.
			vec2<float32> r
		) {
			linearVelocity += j * inverseMass;

			// Subtract to get a clockwise rotation.
			angularVelocity -= parallelogramArea(r, j) * inverseInertia;
		}

		/// Position of the center of mass in world-space.
		vec2<float32> position;

		float32 rotation;

		// This isn't meant to be dynamically updated.
		// TODO(physics/2d/rigid-body/uniform-scaling): Replace with uniform scaling?
		vec2<float32> scale = {1.0f, 1.0f};

		mat3x3<float32> world = identity<mat3x3<float32>>();

		vec2<float32> linearVelocity;

		float32 angularVelocity;

		float32 inverseMass;
		float32 inverseInertia;

		// TODO: Differentiate static friction from dynamic friction.
		/// Coefficient of friction.
		float32 friction;

		float32 restitution;

		// TODO: Keep inside the rigid body?
		vec2<float32> force;
	};
}