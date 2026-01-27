// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

namespace Trident {
	enum class DynamicState : uint8 {
		VIEWPORT = BIT(0),
		SCISSOR = BIT(1),
		LINE_WIDTH = BIT(2),
	};

	AS_FLAG(DynamicState);
}