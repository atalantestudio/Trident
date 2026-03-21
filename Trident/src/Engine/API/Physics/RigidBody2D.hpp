// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"

namespace Trident {
	struct RigidBody2DDescriptor {
		vec2<float32> position = {0.0_m, 0.0_m};
		float32 rotation = 0.0_rad;
		vec2<float32> scale = {1.0f, 1.0f};
		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;
		float32 mass = 0.0_kg;
		float32 friction = 0.0f;
		float32 restitution = 0.0f;
		ref<Geometry2D> geometry;
		float32 density = 1.0f;
		Color fillColor = 0xfffffff;
		Color strokeColor = 0x000000ff;

		// TODO: Add layer/island system to scene ECS.
		// uint32 layerIndex = 0;

		sequence<char8> label;
	};

	class RigidBody2D {
		public:
			void updateTransform() {
				transform =
					mat3x3<float32>::translation(-centerOfMass) *
					mat3x3<float32>::scaling(scale) *
					mat3x3<float32>::rotation(-rotation) *
					mat3x3<float32>::translation(position);
			}

			/// Applies a linear force to the body.
			void applyLinearForce(vec2<float32> F) {
				force += F;
			}

			/// Applies a linear impulse to the body.
			void applyImpulse(vec2<float32> j) {
				linearVelocity += j * inverseMass;
			}

			/// Applies linear and angular impulses to the body.
			void applyImpulse(
				/// The impulse vector.
				vec2<float32> j,

				/// The vector from the entity's position to the contact.
				vec2<float32> r
			) {
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

			// TODO(debug-only)
			sequence<char8> label;
	};
}