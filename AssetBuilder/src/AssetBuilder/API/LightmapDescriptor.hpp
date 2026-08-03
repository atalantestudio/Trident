// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	struct LightmapDescriptor {
		svec2<float32> luxelScale;

		// TODO(asset-builder/lightmapper): Use.
		svec3<float32> tintColor;

		// TODO(asset-builder/lightmapper): Use.
		svec3<float32> emissiveColor;

		// TODO(asset-builder/lightmapper): Use.
		float32 emissiveIntensity;
	};
}