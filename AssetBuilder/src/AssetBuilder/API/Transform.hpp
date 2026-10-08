// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct BuildTransform3D {
		vec3<float32> translation;

		/// In Euler angles.
		vec3<float32> rotation;

		/// Uniform scaling multiplier.
		float32 scale;
	};
}