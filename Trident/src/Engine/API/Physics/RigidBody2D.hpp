// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Geometry/Geometry2D.hpp"
#include "Engine/API/Physics/Cached/CachedRigidBody2D.hpp"

namespace Trident {
	struct RigidBody2DDescriptor {
		vec2<float32> position = {0.0_m, 0.0_m};
		float32 rotation = 0.0_rad;
		vec2<float32> scale = {1.0f, 1.0f};
		vec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;
		float32 mass = 0.0_kg;
		float32 friction = 0.0f;
		float32 density = 1.0f;
		Color fillColor = 0xffffffff;
		Color strokeColor = 0x000000ff;

		// TODO: Add layer/island system to scene ECS.
		// uint32 layerIndex = 0;
	};

	class RigidBody2D {
		public:
			static RigidBody2D fromCached(const CachedRigidBody2D& cachedRigidBody, vec2<float32> centroid) {
				RigidBody2D rigidBody = {
					.position = cachedRigidBody.position,
					.rotation = cachedRigidBody.rotation,
					.scale = cachedRigidBody.scale,
					.inverseMass = cachedRigidBody.inverseMass,
					.inverseInertia = cachedRigidBody.inverseInertia,
					.friction = cachedRigidBody.friction,
				};

				rigidBody.updateTransform(centroid);

				return rigidBody;
			}

		public:
			void updateTransform(vec2<float32> centroid) {
				transform =
					mat3x3<float32>::translation(-centroid) *
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

			// Sum of the forces acting on the object's linear properties.
			vec2<float32> force = {0.0_N, 0.0_N};

			float32 rotation = 0.0_rad;

			float32 angularVelocity = 0.0_w;

			// Torque = inertia * angular acceleration (around the COM).
			float32 torque = 0.0_Nm;

			// NOTE: The scale of an entity isn't meant to be dynamically updated.
			vec2<float32> scale = {1.0f, 1.0f};

			float32 inverseMass = 1.0f / 1.0_kg;

			// Inertia = mass distribution over the shape.
			float32 inverseInertia = 1.0_kg_m2;

			// Coefficient of friction.
			// TODO: Differentiate static friction from dynamic friction.
			float32 friction = 0.0f;
	};
}