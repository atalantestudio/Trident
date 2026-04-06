// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Axis.hpp"

namespace Trident {
	struct RigidBody3DComponent {
		// TODO: Pass the hull centroid.
		void updateWorld() {
			world =
				mat4x4<float32>::scaling(scale) *
				mat4x4<float32>::rotation(RIGHT, -rotation[0]) *
				mat4x4<float32>::rotation(UP, -rotation[1]) *
				mat4x4<float32>::rotation(FORWARD, -rotation[2]) *
				mat4x4<float32>::translation(position);
		}

		// TODO: Use svec3<float32> instead.
		/// Position of the center of mass in world-space.
		vec3<float32> position;

		// TODO: Use svec3<float32> instead.
		vec3<float32> rotation;

		// TODO: Use svec3<float32> instead.
		// NOTE: This isn't meant to be dynamically updated.
		vec3<float32> scale = {1.0f, 1.0f, 1.0f};

		mat4x4<float32> world = mat4x4<float32>::identity();
		quat<float32> orientation = quat<float32>::identity();

		// TODO: Use svec2<float32> instead.
		vec3<float32> linearVelocity;

		// TODO: Use svec2<float32> instead.
		vec3<float32> angularVelocity;

		float32 inverseMass;
		mat3x3<float32> inverseInertia;

		// TODO: Add friction, restitution and forces.
	};
}