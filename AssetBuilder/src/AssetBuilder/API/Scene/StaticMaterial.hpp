// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct StaticMaterial {
		vec4<float32> albedo;

		vec4<float32> emissiveColor;
		float32 emissiveStrength;
	};
}