#pragma once

namespace ProjectA {
	enum class EventCategory : uint8 {
		NONE = 0,
		APPLICATION = BIT(0),
		INPUT = BIT(1),
		KEYBOARD = BIT(2),
		MOUSE = BIT(3),
		MOUSE_BUTTON = BIT(4),
		WINDOW = BIT(5),
	};

	AS_FLAG(EventCategory);
}