#pragma once

#include "Engine/API/Event/Event.hpp"

namespace Trident {
	class WindowCloseEvent : public Event {
		public:
			explicit WindowCloseEvent() = default;

			EVENT_TYPE(EventType::WINDOW_CLOSE)
			EVENT_CATEGORY(EventCategory::WINDOW)
	};
}