// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"

namespace Trident {
	class MouseMoveEvent : public Event {
		public:
			explicit MouseMoveEvent(sint32 x, sint32 y) :
				x(x),
				y(y)
			{}

			sint32 getX() const {
				return x;
			}

			sint32 getY() const {
				return y;
			}

			EVENT_TYPE(EventType::MOUSE_MOVE)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

		private:
			/// If the window is pointer-locked, this is the pointer delta position since the previous movement.
			/// Otherwise, this is the current pointer position.
			// TODO: Use vec2<sint32>.
			const sint32 x;
			const sint32 y;
	};
}