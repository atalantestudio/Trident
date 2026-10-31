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
			return transpose(inverse(mat3x3<float32>{
				world[0][0], world[0][1], world[0][2],
				world[1][0], world[1][1], world[1][2],
				world[2][0], world[2][1], world[2][2],
			}));
		}

		void updateOrientation() {
			const quat<float32> pitch = rotationAxisAngle<quat<float32>>(RIGHT, rotation[0]);
			const quat<float32> yaw = rotationAxisAngle<quat<float32>>(UP, rotation[1]);
			const quat<float32> roll = rotationAxisAngle<quat<float32>>(FORWARD, rotation[2]);

			orientation = normalized(yaw * pitch * roll);
		}

		void updateWorld(vec3<float32> centroid) {
			world = (
				translation<mat4x4<float32>>(-centroid) *
				uniformScaling<mat4x4<float32>>(scale) *
				mat4x4<float32>(orientation) *
				translation<mat4x4<float32>>(position)
			);
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

		/// Position of the center of mass in world-space.
		vec3<float32> position;

		vec3<float32> rotation;

		/// Uniform scaling multiplier.
		float32 scale = 1.0f;

		mat4x4<float32> world = identity<mat4x4<float32>>();
		quat<float32> orientation = identity<quat<float32>>();

		vec3<float32> linearVelocity = {0.0_m_s, 0.0_m_s, 0.0_m_s};

		vec3<float32> angularVelocity = {0.0_rad, 0.0_rad, 0.0_rad};

		float32 inverseMass;
		mat3x3<float32> inverseInertia;

		float32 friction;
		float32 restitution;

		vec3<float32> force = {0.0_N, 0.0_N, 0.0_N};

		/// The entity on which this body is currently resting.
		Entity groundEntity = max<Entity>();
	};
}