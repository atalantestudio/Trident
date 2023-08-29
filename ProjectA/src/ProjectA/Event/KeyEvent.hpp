#pragma once

#include <sstream>
#include "Event.hpp"

namespace ProjectA {
	class API KeyEvent : public Event {
		protected:
			int keyCode;

			KeyEvent(int keyCode) : keyCode(keyCode) {}

		public:
			EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

			inline int getKeyCode() const {
				return keyCode;
			}
	};

	class API KeyPressedEvent : public KeyEvent {
		private:
			int pressCount;

		public:
			KeyPressedEvent(int keyCode, int pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(EventTypeKeyPressed)

			inline int getPressCount() const {
				return pressCount;
			}

			std::string toString() const override {
				std::stringstream stream;
				stream << getName() << ": " << getKeyCode() << " (" << getPressCount() << " repeats)";

				return stream.str();
			}
	};

	class API KeyReleasedEvent : public KeyEvent {
		public:
			KeyReleasedEvent(int keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventTypeKeyReleased)

			std::string toString() const override {
				std::stringstream stream;
				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};
}