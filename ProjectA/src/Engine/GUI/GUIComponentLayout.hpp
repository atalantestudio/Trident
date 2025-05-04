#pragma once

namespace ProjectA {
	enum class GUIComponentLayout : uint8 {
		STRETCH_X = BIT(0),
		STRETCH_Y = BIT(1),
	};

	AS_FLAG(GUIComponentLayout);
}