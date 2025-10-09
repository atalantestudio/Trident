#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"

namespace ProjectA {
	class KeyTypeEvent : public Event {
		public:
			explicit KeyTypeEvent(KeyCode keyCode) :
				keyCode(keyCode)
			{}

			EVENT_TYPE(EventType::KEY_TYPE)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}

		private:
			KeyCode keyCode;
	};
}