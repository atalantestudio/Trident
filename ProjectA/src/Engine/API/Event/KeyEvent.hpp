#pragma once

#include "Engine/API/Event/Event.hpp"

// TODO: Refactor (+ create Event/Keyboard directory).
namespace ProjectA {
	class KeyEvent : public Event {
		public:
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::KEYBOARD)

			KeyCode getKeyCode() const {
				return keyCode;
			}

		protected:
			KeyCode keyCode;

			explicit KeyEvent(KeyCode keyCode) : keyCode(keyCode) {}
	};

	class KeyDownEvent : public KeyEvent {
		public:
			explicit KeyDownEvent(KeyCode keyCode, uint32 repeatCount) :
				KeyEvent(keyCode),
				repeatCount(repeatCount)
			{}

			EVENT_TYPE(EventType::KEY_DOWN)

			uint32 getRepeatCount() const {
				return repeatCount;
			}

		private:
			uint32 repeatCount;
	};

	class KeyUpEvent : public KeyEvent {
		public:
			explicit KeyUpEvent(KeyCode keyCode) :
				KeyEvent(keyCode)
			{}

			EVENT_TYPE(EventType::KEY_UP)
	};

	class KeyTypeEvent : public KeyEvent {
		public:
			explicit KeyTypeEvent(KeyCode keyCode) :
				KeyEvent(keyCode)
			{}

			EVENT_TYPE(EventType::KEY_TYPE)
	};
}