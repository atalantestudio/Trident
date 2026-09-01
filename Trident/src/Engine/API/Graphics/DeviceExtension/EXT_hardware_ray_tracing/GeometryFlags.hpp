// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#if defined(OPAQUE)
	#undef OPAQUE
#endif

namespace Trident {
	enum class GeometryFlags : uint8 {
		OPAQUE = BIT(0),
		NO_DUPLICATE_ANY_HIT_INVOCATION = BIT(1),
	};

	AS_FLAG(GeometryFlags);
}