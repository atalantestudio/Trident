#pragma once

#include "ProjectA/Event/Event.hpp"
#include "ProjectA/Input.hpp"

namespace ProjectA {
	class KeyEvent : public Event {
		protected:
			KeyCode keyCode;

			KeyEvent(KeyCode keyCode) : keyCode(keyCode) {}

		public:
			EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

			KeyCode getKeyCode() const {
				return keyCode;
			}
	};

	class KeyPressEvent : public KeyEvent {
		private:
			int32 pressCount;

		public:
			KeyPressEvent(KeyCode keyCode, int32 pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(EventTypeKeyPress)

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

			EVENT_TYPE(EventTypeKeyRelease)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};

	class KeyTypeEvent : public KeyEvent {
		public:
			KeyTypeEvent(KeyCode keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventTypeKeyType)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};
}