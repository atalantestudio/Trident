// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

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