#pragma once

#include "Event.hpp"

namespace ProjectA {
	class API MouseButtonEvent : public Event {
		protected:
			int button;

			MouseButtonEvent(int button) : button(button) {}

		public:
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			inline int getButton() const {
				return button;
			}
	};

	class API MouseButtonPressEvent : public MouseButtonEvent {
		public:
			MouseButtonPressEvent(int button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonPress)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class API MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(int button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonRelease)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class API MouseMoveEvent : public Event {
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

	class API MouseScrollEvent : public Event {
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