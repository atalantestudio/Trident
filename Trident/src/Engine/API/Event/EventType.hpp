#pragma once

namespace Trident {
	enum class EventType : uint8 {
		NONE,
		KEY_DOWN,
		KEY_UP,
		KEY_TYPE,
		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
		MOUSE_WHEEL,
		WINDOW_CLOSE,
		WINDOW_RESIZE,
	};
}