#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class WindowEvent : public Event {
		protected:
			WindowEvent() = default;

		public:
			EVENT_CATEGORY(EventCategory::WINDOW)
	};
}