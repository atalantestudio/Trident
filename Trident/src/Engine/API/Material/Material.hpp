// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Filesystem/Asset/MaterialAsset.hpp"

namespace Trident {
	struct MaterialGeometry {
		range<uint32, uint32> instanceRange;
		uint32 geometryIndex;
	};

	struct Material {
		ref<MaterialAsset> asset;
		range<uint32, uint32> geometryRange;
	};
}