// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapDescriptor {
		svec2<float32> luxelScale;
		svec3<float32> emissiveColor = {0.0f, 0.0f, 0.0f};
		float32 emissiveIntensity = 0.0f;
		svec3<float32> albedo = {1.0f, 1.0f, 1.0f};
	};
}