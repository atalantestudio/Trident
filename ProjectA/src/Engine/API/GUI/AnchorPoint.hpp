#pragma once

namespace ProjectA {
	enum class AnchorPoint : uint8 {
		TOP = BIT(0),
		VCENTER = BIT(1),
		BOTTOM = BIT(2),
		LEFT = BIT(3),
		HCENTER = BIT(4),
		RIGHT = BIT(5),
	};

	AS_FLAG(AnchorPoint);
}