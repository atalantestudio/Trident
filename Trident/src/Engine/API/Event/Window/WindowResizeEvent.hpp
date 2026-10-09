// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/Event.hpp"

namespace Trident {
	class WindowResizeEvent : public Event {
		public:
			explicit WindowResizeEvent(vec2<sint32> extent) :
				extent(extent)
			{}

			EVENT_TYPE(EventType::WINDOW_RESIZE)
			EVENT_CATEGORY(EventCategory::WINDOW)

			vec2<sint32> getExtent() const {
				return extent;
			}

		private:
			vec2<sint32> extent;
	};
}