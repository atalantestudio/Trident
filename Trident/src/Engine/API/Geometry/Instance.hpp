// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct Instance3D {
		mat4x4<float32> worldTransform;
		mat3x3<float32> normalTransform;
	};
}