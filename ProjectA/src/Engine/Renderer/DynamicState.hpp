#pragma once

namespace ProjectA {
	enum class DynamicState : uint8 {
		VIEWPORT = BIT(0),
		SCISSOR = BIT(1),
		PRIMITIVE_TOPOLOGY = BIT(2),
	};

	AS_FLAG(DynamicState);
}