#pragma once

#include <sstream>
#include "Event.hpp"

namespace ProjectA {
	class API KeyEvent : public Event {
		protected:
			int keyCode;

			KeyEvent(int keyCode) : keyCode(keyCode) {}

		public:
			EVENT_CATEGORY(EventCategory::Input | EventCategory::Keyboard)

			inline int getKeyCode() const {
				return keyCode;
			}
	};

	class API KeyPressedEvent : public KeyEvent {
		private:
			int pressCount;

		public:
			KeyPressedEvent(int keyCode, int pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(KeyPressed)

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

			EVENT_TYPE(KeyReleased)

			std::string toString() const override {
				std::stringstream stream;
				stream << getName() << ": " << getKeyCode();

				return stream.str();
			}
	};
}