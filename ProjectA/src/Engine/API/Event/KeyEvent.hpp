#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	class KeyEvent : public Event {
		protected:
			KeyCode keyCode;

			KeyEvent(KeyCode keyCode) : keyCode(keyCode) {}

		public:
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}
	};

	class KeyPressEvent : public KeyEvent {
		private:
			sint32 pressCount;

		public:
			KeyPressEvent(KeyCode keyCode, sint32 pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(EventType::KEY_PRESS)

			sint32 getPressCount() const {
				return pressCount;
			}
	};

	class KeyReleaseEvent : public KeyEvent {
		public:
			KeyReleaseEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventType::KEY_RELEASE)
	};

	class KeyTypeEvent : public KeyEvent {
		public:
			KeyTypeEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventType::KEY_TYPE)
	};
}