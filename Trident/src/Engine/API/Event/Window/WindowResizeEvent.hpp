// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"

namespace Trident {
	class WindowResizeEvent : public Event {
		public:
			// TODO(lyah/integer-vector)
			explicit WindowResizeEvent(sint32 width, sint32 height) :
				width(width),
				height(height)
			{}

			EVENT_TYPE(EventType::WINDOW_RESIZE)
			EVENT_CATEGORY(EventCategory::WINDOW)

			sint32 getWidth() const {
				return width;
			}

			sint32 getHeight() const {
				return height;
			}

		private:
			// TODO(lyah/integer-vector): Use vec2<sint32>.
			sint32 width;
			sint32 height;
	};
}