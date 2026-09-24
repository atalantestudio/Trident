// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapDescriptor {
		/// Size of a pixel on the lightmap. Larger pixels mean smaller resolution lightmaps.
		svec2<float32> luxelScale = {1.0f, 1.0f};

		svec3<float32> emissiveColor = {0.0f, 0.0f, 0.0f};
		float32 emissiveIntensity = 0.0f;
		svec3<float32> albedo = {1.0f, 1.0f, 1.0f};
	};
}