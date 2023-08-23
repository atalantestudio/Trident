#pragma once

#include "Event.hpp"

namespace ProjectA {
	class API KeyEvent : public Event {
		public:
			virtual int getCategories() const override {
				return EventCategory::Input | EventCategory::Keyboard;
			}

		protected:
			int keyCode;

			KeyEvent(int keyCode) : keyCode(keyCode) {}

		public:
			inline int getKeyCode() const {
				return keyCode;
			}
	};

	class API KeyPressedEvent : public KeyEvent {
		public:
			virtual EventType getType() const {
				return EventType::KeyPressed;
			}

		private:
			int pressCount;

		public:
			KeyPressedEvent(int keyCode, int pressCount): KeyEvent(keyCode), pressCount(pressCount) {}

			inline int getPressCount() const {
				return pressCount;
			}
	};
}