// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct PointLight {
		svec3<float32> color;
		float32 intensity;
		svec3<float32> position;
		uint32 _;
	};
}