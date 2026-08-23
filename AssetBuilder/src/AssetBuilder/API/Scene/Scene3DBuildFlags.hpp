// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class Scene3DBuildFlags : uint8 {
		/// Whether to trace rays towards the sun while generating the lightmaps.
		LIGHTMAP_SUN_CONTRIBUTION = BIT(0),
	};

	AS_FLAG(Scene3DBuildFlags);
}