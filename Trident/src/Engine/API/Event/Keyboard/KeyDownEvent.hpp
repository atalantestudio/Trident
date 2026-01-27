#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"

namespace Trident {
	class KeyDownEvent : public Event {
		public:
			explicit KeyDownEvent(KeyCode keyCode) :
				keyCode(keyCode)
			{}

			EVENT_TYPE(EventType::KEY_DOWN)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}

		private:
			KeyCode keyCode;
	};
}