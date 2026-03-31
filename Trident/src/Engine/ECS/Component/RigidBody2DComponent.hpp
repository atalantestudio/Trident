// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct RigidBody2DComponent {
		void updateTransform(vec2<float32> centroid) {
			transform =
				mat3x3<float32>::translation(-centroid) *
				mat3x3<float32>::scaling(scale) *
				mat3x3<float32>::rotation(-rotation) *
				mat3x3<float32>::translation(position);
		}

		/// Position of the center of mass in world-space.
		svec2<float32> position;

		float32 rotation;

		// NOTE: This isn't meant to be dynamically updated.
		svec2<float32> scale;

		mat3x3<float32> transform;

		svec2<float32> linearVelocity;
		float32 angularVelocity;

		float32 inverseMass;
		float32 inverseInertia;

		// TODO: Differentiate static friction from dynamic friction.
		/// Coefficient of friction.
		float32 friction;

		float32 restitution;
	};
}