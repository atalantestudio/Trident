#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/MouseButton.hpp"

namespace Trident {
	class MouseUpEvent : public Event {
		public:
			explicit MouseUpEvent(MouseButton button) :
				button(button)
			{}

			MouseButton getButton() const {
				return button;
			}

			EVENT_TYPE(EventType::MOUSE_UP)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

		private:
			MouseButton button;
	};
}