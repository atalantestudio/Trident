#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class MouseWheelEvent : public Event {
		public:
			explicit MouseWheelEvent(sint16 delta) :
				delta(delta)
			{}

			sint16 getDelta() const {
				return delta;
			}

			EVENT_TYPE(EventType::MOUSE_WHEEL)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

		private:
			sint16 delta;
	};
}