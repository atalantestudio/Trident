#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Base/KeyCode.hpp"

namespace ProjectA {
	class KeyDownEvent : public Event {
		public:
			explicit KeyDownEvent(KeyCode keyCode, uint16 repeatCount) :
				keyCode(keyCode),
				repeatCount(repeatCount)
			{}

			EVENT_TYPE(EventType::KEY_DOWN)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}

			uint16 getRepeatCount() const {
				return repeatCount;
			}

		private:
			KeyCode keyCode;
			uint16 repeatCount;
	};
}