#pragma once

#include "Engine/Event/Event.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	class MouseButtonEvent : public Event {
		protected:
			MouseButton button;

			MouseButtonEvent(MouseButton button) : button(button) {}

		public:
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

			MouseButton getButton() const {
				return button;
			}
	};

	class MouseButtonPressEvent : public MouseButtonEvent {
		public:
			MouseButtonPressEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventType::BUTTON_PRESS)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventType::BUTTON_RELEASE)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class MouseMoveEvent : public Event {
		private:
			lyah::vec2<float32> position;

		public:
			MouseMoveEvent(float32 x, float32 y) : position(x, y) {}

			const lyah::vec2<float32>& getPosition() const {
				return position;
			}

			EVENT_TYPE(EventType::MOUSE_MOVE)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << position[0] << ", " << position[1];

				return stream.str();
			}
	};

	class MouseScrollEvent : public Event {
		private:
			lyah::vec2<float32> offset;

		public:
			MouseScrollEvent(float32 xOffset, float32 yOffset) : offset(xOffset, yOffset) {}

			const lyah::vec2<float>& getOffset() const {
				return offset;
			}

			EVENT_TYPE(EventType::MOUSE_SCROLL)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << offset[0] << ", " << offset[1];

				return stream.str();
			}
	};
}