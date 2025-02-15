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

			MouseButton getButton() const {
				return button;
			}
	};

	class MouseButtonPressEvent : public MouseButtonEvent {
		public:
			MouseButtonPressEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonPress)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(MouseButton button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonRelease)

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

			EVENT_TYPE(EventTypeMouseMove)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

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

			EVENT_TYPE(EventTypeMouseScroll)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << offset[0] << ", " << offset[1];

				return stream.str();
			}
	};
}