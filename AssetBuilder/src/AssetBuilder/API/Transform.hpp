// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct BuildTransform3D {
		svec3<float32> translation;

		/// In Euler angles.
		svec3<float32> rotation;

		/// Uniform scaling multiplier.
		float32 scale;
	};

	struct TransformAngleAxis3D {
		/// Relative to the surface center.
		vec3<float32> translation;

		/// Rotation axis.
		vec3<float32> axis;

		/// Rotation angle in radians.
		float32 angle;
	};
}