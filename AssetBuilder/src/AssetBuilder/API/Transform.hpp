// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Transform {
		/// Relative to the surface center.
		vec3<float32> translation;

		/// Rotation axis.
		vec3<float32> axis;

		/// Rotation angle in radians.
		float32 angle;
	};

	struct EulerTransform {
		vec3<float32> translation;
		vec3<float32> eulerAngles;
		vec3<float32> scale;
	};
}