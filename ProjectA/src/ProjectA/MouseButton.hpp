#pragma once

namespace ProjectA {
	enum class MouseButton : uint8 {
		BUTTON_0 = 0,
		BUTTON_1 = 1,
		BUTTON_2 = 2,
		BUTTON_3 = 3,
		BUTTON_4 = 4,
		BUTTON_5 = 5,
		BUTTON_6 = 6,
		BUTTON_7 = 7,

		// Aliases
		BUTTON_LAST = BUTTON_7,
		BUTTON_LEFT = BUTTON_0,
		BUTTON_RIGHT = BUTTON_1,
		BUTTON_MIDDLE = BUTTON_2
	};

	AS_PRINTABLE(MouseButton);
}