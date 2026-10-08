// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct PointLight {
		vec3<float32> color;
		float32 intensity;
		alignas(16) vec3<float32> position;
	};
}