// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class Hull2DType : uint8 {
		POINT,
		CIRCLE,
		POLYGON,

		count,
	};

	static constexpr uint32 MAX_HULL_2D_TYPE_COUNT = static_cast<uint32>(Hull2DType::count);
}