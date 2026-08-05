// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class GeometryAssetFlags : uint8 {
		IS_3D = BIT(0),

		/// If set, the asset contains a hull.
		HAS_HULL = BIT(1),
	};

	AS_FLAG(GeometryAssetFlags);
}