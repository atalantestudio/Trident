#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
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
			// - `F` is the linear force added to the body.
			void applyLinearForce(vec2<float32> F) {
				this->force += F;
			}

			// Applies a linear and angular impulse to the body.
			// - `j` is the impulse vector.
			// - `r` is the vector from the entity's position to the contact (referred to as "radius").
			void applyImpulse(vec2<float32> j, vec2<float32> r) {
				linearVelocity += j * inverseMass;
				angularVelocity += parallelogramArea(r, j) * inverseInertia;
			}

		public:
			mat3x3<float32> transform = mat3x3<float32>::identity();

			// Position of the center of mass, in world-space.
			vec2<float32> position = {0.0_m, 0.0_m};

			vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};

			// TODO: Use linear acceleration with Verlet integration.
			vec2<float32> linearAcceleration = {0.0_m_s2, 0.0_m_s2};

			// Sum of the forces acting on the object's linear properties.
			vec2<float32> force = {0.0_N, 0.0_N};

			float32 rotation = 0.0_rad;

			float32 angularVelocity = 0.0_w;

			// Torque = inertia * angular acceleration (around the COM).
			float32 torque = 0.0_Nm;

			// NOTE: The scale of an entity isn't meant to be dynamically updated.
			vec2<float32> scale = {1.0f, 1.0f};

			float32 inverseMass = 1.0f / 1.0_kg;

			vec2<float32> centerOfMass;

			// Coefficient of friction.
			// TODO: Differentiate static friction from dynamic friction.
			float32 friction = 0.0f;

			// Inertia = mass distribution over the shape.
			float32 inverseInertia = 1.0_kg_m2;

			ref<Geometry2D> geometry;
	};
}