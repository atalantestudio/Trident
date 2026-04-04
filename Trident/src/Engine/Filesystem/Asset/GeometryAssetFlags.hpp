// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class GeometryAssetFlags : uint8 {
		IS_2D = BIT(0),
		IS_3D = BIT(1),

		/// If set, the geometry data is followed by a `Hull2DHeader` structure.
		HAS_HULL = BIT(2),
	};

	AS_FLAG(GeometryAssetFlags);
}