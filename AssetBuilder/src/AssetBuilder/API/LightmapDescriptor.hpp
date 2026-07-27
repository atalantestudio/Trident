// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapDescriptor {
		svec2<float32> luxelScale;
		svec3<float32> emissiveColor;
		float32 emissiveIntensity;
		svec3<float32> tintColor;
	};
}