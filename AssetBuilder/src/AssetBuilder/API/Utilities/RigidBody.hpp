// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

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
					ENGINE_ASSERT(component.scale.x == component.scale.y, "Ellipsoid shapes are not supported.");

					const float32 r = component.scale.x;

					// The object is assumed to have a unit circle geometry. The same scaling must be applied to both axes.
					I = m * r * r / 2.0f;

					break;
				}
				case MomentOfInertia2D::THIN_RECTANGULAR_PLATE: {
					const float32 area = dot(component.scale, component.scale);

					// The object is assumed to have a unit square geometry. The axis of rotation is at the center.
					I = m * area / 12.0f;

					break;
				}
				case MomentOfInertia2D::POLYGON: {
					float32 area = 0.0f;
					index i = 0;

					while (i < descriptor.geometry->getPositionCount()) {
						const index j = (i + 1) % descriptor.geometry->getPositionCount();
						const vec2<float32> Pi = vec2<float32>{
							descriptor.geometry->vertexPositionComponents[i * 2 + 0],
							descriptor.geometry->vertexPositionComponents[i * 2 + 1],
						} - descriptor.geometry->hullCentroid;
						const vec2<float32> Pj = vec2<float32>{
							descriptor.geometry->vertexPositionComponents[j * 2 + 0],
							descriptor.geometry->vertexPositionComponents[j * 2 + 1],
						} - descriptor.geometry->hullCentroid;

						const float32 Qi = lengthSquared(Pi) + dot(Pi, Pj) + lengthSquared(Pj);
						const float32 a = parallelogramArea(Pj, Pi);

						I += Qi * a;
						area += a;
						i += 1;
					}

					I = m * (I / (6.0f * area));

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
					// The object is assumed to have a unit square geometry with the axis of rotation at the center.

					// TODO(physics/3d/inertia): Calculate the size without using the scale.
					const vec3<float32> inertia = descriptor.inertiaScale * component.scale;

					const float32 w = inertia.x;
					const float32 h = inertia.y;
					const float32 d = inertia.z;

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