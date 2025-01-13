#pragma once

#include "ProjectA/Event/Event.hpp"
#include "ProjectA/Input.hpp"

namespace ProjectA {
	class MouseButtonEvent : public Event {
		protected:
			MouseButton button;

			MouseButtonEvent(MouseButton button) : button(button) {}

		public:
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			inline MouseButton getButton() const {
				return button;
			}
	};

	class MouseButtonPressEvent : public MouseButtonEvent {
		public:
			MouseButtonPressEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonPress)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonRelease)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class MouseMoveEvent : public Event {
		private:
			vec2<float> position;

		public:
			MouseMoveEvent(float x, float y) : position(x, y) {}

			inline const vec2<float>& getPosition() const {
				return position;
			}

			EVENT_TYPE(EventTypeMouseMove)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << position.x << ", " << position.y;

				return stream.str();
			}
	};

	class MouseScrollEvent : public Event {
		private:
			vec2<float> offset;

		public:
			MouseScrollEvent(float xOffset, float yOffset) : offset(xOffset, yOffset) {}

			inline const vec2<float>& getOffset() const {
				return offset;
			}

			EVENT_TYPE(EventTypeMouseScroll)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << offset.x << ", " << offset.y;

				return stream.str();
			}
	};
}