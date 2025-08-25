#pragma once

namespace ProjectA {
	class RigidBody2D {
		public:
			void updateTransform() {
				transform =
					mat3x3<float32>::translation(-centerOfMass) *
					mat3x3<float32>::scaling(scale) *
					mat3x3<float32>::rotation(-rotation) *
					mat3x3<float32>::translation(position);
			}

			// Applies a linear force to the body.
			void applyForce(const vec2<float32>& force) {
				this->force += force;
			}

			// Applies a linear and angular impulse to the body.
			// - `j` is the impulse vector.
			// - `r` is the vector from the entity's position to the contact (referred to as "radius").
			void applyImpulse(const vec2<float32>& j, const vec2<float32>& r) {
				linearVelocity += j * inverseMass;
				angularVelocity += parallelogramArea(r, j) * inverseInertia;
			}

		public:
			mat3x3<float32> transform = mat3x3<float32>::identity();

			// Position of the center of mass, in world-space.
			vec2<float32> position = {0.0_m, 0.0_m};

			// TODO(review): Use correct SI unit.
			vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};

			// TODO(review): Use correct SI unit.
			vec2<float32> linearAcceleration = {0.0_m_s2, 0.0_m_s2};

			// Sum of the forces acting on the object's linear properties.
			// TODO(review): Use correct SI unit.
			vec2<float32> force = {0.0f, 0.0f};

			// TODO(review): Use correct SI unit.
			float32 rotation = 0.0f;

			float32 angularVelocity = 0.0_m_s;

			// Torque = inertia * angular acceleration (around the COM).
			// TODO(review): Use correct SI unit.
			float32 torque = 0.0f;

			// NOTE: The scale of an entity isn't meant to be dynamically updated.
			vec2<float32> scale = {1.0f, 1.0f};

			float32 inverseMass = 1.0f / 1.0_kg;

			vec2<float32> centerOfMass;

			// TODO: Differentiate static friction from dynamic friction.
			// TODO(review): Use correct SI unit.
			float32 friction = 0.0f;

			// Inertia = mass distribution over the shape.
			// TODO(review): Use correct SI unit.
			float32 inverseInertia = 1.0f;

			ref<Geometry2D> geometry;
	};
}