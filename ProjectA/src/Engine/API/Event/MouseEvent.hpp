#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/MouseButton.hpp"

namespace ProjectA {
	class MouseButtonEvent : public Event {
		protected:
			MouseButton button;

			MouseButtonEvent(MouseButton button) : button(button) {}

		public:
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

			MouseButton getButton() const {
				return button;
			}
	};

	class MouseButtonPressEvent : public MouseButtonEvent {
		public:
			MouseButtonPressEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventType::BUTTON_PRESS)
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventType::BUTTON_RELEASE)
	};
}