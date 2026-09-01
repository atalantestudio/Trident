// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class AccelerationStructureType : uint8 {
		/// Specifies a top-level acceleration structure.
		TOP_LEVEL,

		/// Specifies a bottom-level acceleration structure.
		BOTTOM_LEVEL,

		count,
	};
}