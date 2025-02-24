#pragma once

#include "Engine/Event/Event.hpp"
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
			int32 pressCount;

		public:
			KeyPressEvent(KeyCode keyCode, int32 pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(EventType::KEY_PRESS)

			int32 getPressCount() const {
				return pressCount;
			}

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << getKeyCode() << " (" << getPressCount() << " repeats)";

				return stream.str();
			}
	};

	class KeyReleaseEvent : public KeyEvent {
		public:
			KeyReleaseEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventType::KEY_RELEASE)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};

	class KeyTypeEvent : public KeyEvent {
		public:
			KeyTypeEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventType::KEY_TYPE)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};
}