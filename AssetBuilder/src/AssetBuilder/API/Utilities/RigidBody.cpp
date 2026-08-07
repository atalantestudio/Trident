// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "AssetBuilder/API/Utilities/Utilities.hpp"

namespace Trident {
	RigidBody2DComponent createRigidBody(const Descriptor<RigidBody2DComponent>& descriptor) {
		ENGINE_ASSERT(descriptor.mass > 0.0_kg, "The mass must be positive.");
		ENGINE_ASSERT(descriptor.friction >= 0.0f && descriptor.friction <= 1.0f, "The friction must be between 0 and 1.");
		ENGINE_ASSERT(descriptor.restitution >= 0.0f && descriptor.restitution <= 1.0f, "The restitution must be between 0 and 1.");

		RigidBody2DComponent component = {
			.position = descriptor.position,
			.rotation = descriptor.rotation,
			.scale = descriptor.scale,
			.linearVelocity = descriptor.linearVelocity,
			.angularVelocity = descriptor.angularVelocity,
			.inverseMass = 0.0_kg,
			.inverseInertia = 0.0_kg_m2,
			.friction = descriptor.friction,
			.restitution = descriptor.restitution,
		};

		const float32 m = descriptor.mass;

		if (m < max<float32>()) {
			float32 I = 0.0_kg_m2;

			switch (descriptor.momentOfInertia) {
				case MomentOfInertia2D::POINT: {
					I = m;

					break;
				}
				case MomentOfInertia2D::THIN_SOLID_DISK: {
					ENGINE_ASSERT(component.scale[0] == component.scale[1], "Ellipsoid shapes are not supported.");

					const float32 r = component.scale[0];

					// NOTE: The object is assumed to have a unit circle geometry. The same scaling must be applied to both axes.
					I = m * r * r / 2.0f;

					break;
				}
				case MomentOfInertia2D::THIN_RECTANGULAR_PLATE: {
					const float32 w = component.scale[0];
					const float32 h = component.scale[1];

					// NOTE: The object is assumed to have a unit square geometry. The axis of rotation is at the center.
					I = m * (w * w + h * h) / 12.0f;

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			component.inverseMass = 1.0f / m;
			component.inverseInertia = 1.0f / I;
		}

		return component;
	}

	RigidBody3DComponent createRigidBody(const Descriptor<RigidBody3DComponent>& descriptor) {
		ENGINE_ASSERT(descriptor.mass > 0.0_kg, "The mass must be positive.");
		ENGINE_ASSERT(descriptor.friction >= 0.0f && descriptor.friction <= 1.0f, "The friction must be between 0 and 1.");
		ENGINE_ASSERT(descriptor.restitution >= 0.0f && descriptor.restitution <= 1.0f, "The restitution must be between 0 and 1.");

		RigidBody3DComponent component = {
			.position = descriptor.position,
			.rotation = descriptor.rotation,
			.scale = descriptor.uniformScale,
			.linearVelocity = descriptor.linearVelocity,
			.angularVelocity = descriptor.angularVelocity,
			.inverseMass = 0.0_kg,
			.friction = descriptor.friction,
			.restitution = descriptor.restitution,
		};

		const float32 m = descriptor.mass;

		if (m < max<float32>()) {
			mat3x3<float32> I;

			switch (descriptor.momentOfInertia) {
				case MomentOfInertia3D::SOLID_CUBOID: {
					// NOTE: The object is assumed to have a unit square geometry. The axis of rotation is at the center.

					// TODO(physics/3d/inertia): Calculate the size without using the scale.
					const float32 w = descriptor.inertiaScale[0] * component.scale;
					const float32 h = descriptor.inertiaScale[1] * component.scale;
					const float32 d = descriptor.inertiaScale[2] * component.scale;

					I = {
						m / 12.0f * (h * h + d * d), 0.0f, 0.0f,
						0.0f, m / 12.0f * (w * w + d * d), 0.0f,
						0.0f, 0.0f, m / 12.0f * (w * w + h * h),
					};

					break;
				}
				default: {
					DEBUG_ASSERT(false);

					break;
				}
			}

			component.inverseMass = 1.0f / m;
			component.inverseInertia = inverse(I);
		}

		return component;
	}
}