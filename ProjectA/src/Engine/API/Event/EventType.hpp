#pragma once

namespace ProjectA {
	enum class EventType : uint8 {
		NONE = 0,
		APPLICATION_RENDER = 1,
		APPLICATION_TICK = 2,
		APPLICATION_UPDATE = 3,
		KEY_PRESS = 4,
		KEY_RELEASE = 5,
		KEY_TYPE = 6,
		BUTTON_PRESS = 7,
		BUTTON_RELEASE = 8,
		MOUSE_MOVE = 9,
		MOUSE_WHEEL = 10,
		WINDOW_CLOSE = 11,
		WINDOW_RESIZE = 12,
	};
}