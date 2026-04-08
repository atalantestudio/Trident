// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct RigidBody2DComponent {
		void updateWorld(vec2<float32> centroid) {
			world =
				mat3x3<float32>::translation(-centroid) *
				mat3x3<float32>::scaling(scale) *
				mat3x3<float32>::rotation(-rotation) *
				mat3x3<float32>::translation(position);
		}

		/// Applies a linear force to the body.
		inline void applyLinearForce(vec2<float32> F) {
			force += F;
		}

		/// Applies linear and angular impulses to the body.
		inline void applyImpulse(
			/// The impulse vector.
			vec2<float32> j,

			/// The vector from the entity's position to the contact.
			vec2<float32> r
		) {
			linearVelocity += j * inverseMass;
			angularVelocity += parallelogramArea(r, j) * inverseInertia;
		}

		// TODO: Use svec2<float32> instead.
		/// Position of the center of mass in world-space.
		vec2<float32> position;

		float32 rotation;

		// TODO: Use svec2<float32> instead.
		// NOTE: This isn't meant to be dynamically updated.
		vec2<float32> scale = {1.0f, 1.0f};

		mat3x3<float32> world = mat3x3<float32>::identity();

		// TODO: Use svec2<float32> instead.
		vec2<float32> linearVelocity;

		float32 angularVelocity;

		float32 inverseMass;
		float32 inverseInertia;

		// TODO: Differentiate static friction from dynamic friction.
		/// Coefficient of friction.
		float32 friction;

		float32 restitution;

		// TODO: Use svec2<float32> instead.
		// TODO: Keep inside the rigid body?
		vec2<float32> force;
	};
}