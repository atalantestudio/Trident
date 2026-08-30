// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Axis.hpp"
#include "Engine/ECS/Entity.hpp"

namespace Trident {
	struct RigidBody3DComponent {
		inline Transform3D getTransform() const {
			return {position, orientation};
		}

		// TODO(asset/scene/3d/instance): See https://marti.works/posts/post-normal-matrix-optimization/post.
		/// Returns a 3x3 normal matrix calculated from the world matrix of this component.
		inline mat3x3<float32> calculateNormal() const {
			return transpose(inverse(mat3x3<float32>(
				world[0][0], world[0][1], world[0][2],
				world[1][0], world[1][1], world[1][2],
				world[2][0], world[2][1], world[2][2]
			)));
		}

		void updateOrientation() {
			const quat<float32> pitch = quat<float32>::axisAngle(RIGHT, rotation[0]);
			const quat<float32> yaw = quat<float32>::axisAngle(UP, rotation[1]);
			const quat<float32> roll = quat<float32>::axisAngle(FORWARD, rotation[2]);

			orientation = normalized(yaw * pitch * roll);
		}

		void updateWorld(vec3<float32> centroid) {
			world =
				// TODO(rigid-body/3d/centroid): Include the centroid in the world matrix?
				mat4x4<float32>::translation(-centroid) *

				// TODO(rigid-body/3d/uniform-scaling)
				mat4x4<float32>::scaling({scale, scale, scale}) *

				mat4x4<float32>(orientation) *
				mat4x4<float32>::translation(position);
		}

		/// Applies linear and angular impulses to the body.
		inline void applyImpulse(
			/// The impulse vector.
			vec3<float32> j,

			/// The vector from the body's position to the contact.
			vec3<float32> r
		) {
			linearVelocity += j * inverseMass;

			// Subtract to get a clockwise rotation.
			angularVelocity -= cross(r, j) * inverseInertia;
		}

		/// The planes composing the body.
		view<Plane3D> facePlanes;

		// TODO: Use svec3<float32> instead.
		/// Position of the center of mass in world-space.
		vec3<float32> position;

		// TODO: Use svec3<float32> instead.
		vec3<float32> rotation;

		/// Uniform scaling across all directions.
		float32 scale = 1.0f;

		mat4x4<float32> world = mat4x4<float32>::identity();
		quat<float32> orientation = quat<float32>::identity();

		// TODO: Use svec3<float32> instead.
		vec3<float32> linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};

		// TODO: Use svec3<float32> instead.
		vec3<float32> angularVelocity = {0.0_rad, 0.0_rad, 0.0_rad};

		float32 inverseMass;
		mat3x3<float32> inverseInertia;

		float32 friction;
		float32 restitution;

		// TODO: Use svec3<float32> instead.
		vec3<float32> force = {0.0_N, 0.0_N, 0.0_N};

		/// The entity owning the rigid body this body is currently resting on.
		Entity groundEntity = max<Entity>();
	};
}