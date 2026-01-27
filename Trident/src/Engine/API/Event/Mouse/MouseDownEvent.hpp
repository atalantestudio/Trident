#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/MouseButton.hpp"

namespace Trident {
	class MouseDownEvent : public Event {
		public:
			explicit MouseDownEvent(MouseButton button) :
				button(button)
			{}

			MouseButton getButton() const {
				return button;
			}

			EVENT_TYPE(EventType::MOUSE_DOWN)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

		private:
			MouseButton button;
	};
}