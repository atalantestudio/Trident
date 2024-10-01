#pragma once

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

	class API KeyPressEvent : public KeyEvent {
		private:
			int pressCount;

		public:
			KeyPressEvent(int keyCode, int pressCount) : KeyEvent(keyCode), pressCount(pressCount) {}

			EVENT_TYPE(EventTypeKeyPress)

			inline int getPressCount() const {
				return pressCount;
			}

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << getKeyCode() << " (" << getPressCount() << " repeats)";
			}
	};

	class API KeyReleaseEvent : public KeyEvent {
		public:
			KeyReleaseEvent(int keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventTypeKeyRelease)

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << getKeyCode();
			}
	};

	class API KeyTypeEvent : public KeyEvent {
		public:
			KeyTypeEvent(int keyCode) : KeyEvent(keyCode) {}

			EVENT_TYPE(EventTypeKeyType)

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << getKeyCode();
			}
	};
}