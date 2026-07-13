// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct StaticMaterial3D {
		ref<MaterialAsset> materialAsset;
		range<uint32, uint32> triangleRange;

		// TODO(asset-builder/lightmapper): Use when generating the lightmaps.
		// vec4<float32> emissiveColor;
		// float32 emissiveStrength;
	};
}