#pragma once

namespace ProjectA {
	enum class SeparationType : uint8 {
		VERTEX_A = 0,
		VERTEX_B = 0,
		EDGE_A = BIT(0),
		EDGE_B = BIT(1),
	};

	AS_FLAG(SeparationType);
}