#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"

namespace ProjectA {
	class KeyUpEvent : public Event {
		public:
			explicit KeyUpEvent(KeyCode keyCode) :
				keyCode(keyCode)
			{}

			EVENT_TYPE(EventType::KEY_UP)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}

		private:
			KeyCode keyCode;
	};
}