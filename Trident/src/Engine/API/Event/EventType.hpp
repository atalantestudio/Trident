// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

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
		TEXT_INPUT,
		WINDOW_CLOSE,
		WINDOW_RESIZE,
	};
}