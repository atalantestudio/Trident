// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class CullMode : uint8 {
		NONE = 0,
		FRONT = 1,
		BACK = 2,
		FRONT_AND_BACK = 3,

		count,
	};
}