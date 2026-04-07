// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Physics/MomentOfInertia.hpp"
#include "Engine/ECS/Component/RigidBody2DComponent.hpp"

namespace Trident {
	template<>
	struct Descriptor<RigidBody2DComponent> {
		svec2<float32> centroid;
		svec2<float32> position;
		float32 rotation;
		svec2<float32> scale;
		svec2<float32> linearVelocity = {0.0_m_s, 0.0_m_s};
		float32 angularVelocity = 0.0_w;
		float32 mass;
		float32 friction;
		float32 restitution;
		MomentOfInertia2D momentOfInertia;
	};

	inline RigidBody2DComponent createRigidBody(const Descriptor<RigidBody2DComponent>& descriptor) {
		ENGINE_ASSERT(descriptor.mass > 0.0_kg, "Mass must be positive.");
		ENGINE_ASSERT(descriptor.friction >= 0.0f && descriptor.friction <= 1.0f, "Friction must be between 0 and 1.");
		ENGINE_ASSERT(descriptor.restitution >= 0.0f && descriptor.restitution <= 1.0f, "Restitution must be between 0 and 1.");

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

			// See https://en.wikipedia.org/wiki/List_of_moments_of_inertia.
			switch (descriptor.momentOfInertia) {
				case MomentOfInertia2D::POINT: {
					I = m;

					break;
				}
				case MomentOfInertia2D::THIN_SOLID_DISK: {
					ENGINE_ASSERT(component.scale[0] == component.scale[1], "Ellipsoid shapes are not supported.");

					const float32 r = component.scale[0];

					// NOTE: Object is assumed to have a unit circle geometry. The same scaling must be applied to both axes.
					I = m * r * r / 2.0f;

					break;
				}
				case MomentOfInertia2D::THIN_RECTANGULAR_PLATE: {
					const float32 w = component.scale[0];
					const float32 h = component.scale[1];

					// NOTE: Object is assumed to have a unit square geometry. The axis of rotation is at the center.
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

		component.updateWorld(descriptor.centroid);

		return component;
	}
}