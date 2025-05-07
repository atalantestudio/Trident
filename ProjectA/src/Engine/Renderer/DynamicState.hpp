#pragma once

namespace ProjectA {
	enum class DynamicState : uint8 {
		VIEWPORT = BIT(0),
		SCISSOR = BIT(1),
	};

	AS_FLAG(DynamicState);
}