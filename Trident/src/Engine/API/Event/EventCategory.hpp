#pragma once

namespace Trident {
	enum class EventCategory : uint8 {
		NONE = 0,
		INPUT = BIT(0),
		KEYBOARD = BIT(1),
		MOUSE = BIT(2),
		MOUSE_BUTTON = BIT(3),
		WINDOW = BIT(4),
	};

	AS_FLAG(EventCategory);
}