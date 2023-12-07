#pragma once

#include <sstream>
#include "Event.hpp"

namespace ProjectA {
	class API MouseMoveEvent : public Event {
		private:
			float x, y;

		public:
			MouseMoveEvent(float x, float y) : x(x), y(y) {}

			inline float getX() const {
				return x;
			}

			inline float getY() const {
				return y;
			}

			EVENT_TYPE(EventTypeMouseMove)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			std::string toString() const override {
				std::stringstream stream;
				stream << getName() << ": " << x << ", " << y;

				return stream.str();
			}
	};

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

			std::string toString() const {
				std::stringstream stream;
				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class API MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(int button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonRelease)

			std::string toString() const {
				std::stringstream stream;
				stream << getName() << ": " << button;

				return stream.str();
			}
	};

	class API MouseScrollEvent : public Event {
		private:
			float xOffset, yOffset;

		public:
			MouseScrollEvent(float xOffset, float yOffset) : xOffset(xOffset), yOffset(yOffset) {}

			inline float getXOffset() const {
				return xOffset;
			}

			inline float getYOffset() const {
				return yOffset;
			}

			EVENT_TYPE(EventTypeMouseScroll)
			EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

			std::string toString() const override {
				std::stringstream stream;
				stream << getName() << ": " << xOffset << ", " << yOffset;

				return stream.str();
			}
	};
}