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

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << button;
			}
	};

	class API MouseButtonReleaseEvent : public MouseButtonEvent {
		public:
			MouseButtonReleaseEvent(int button) : MouseButtonEvent(button) {}

			EVENT_TYPE(EventTypeMouseButtonRelease)

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << button;
			}
	};

	/// TODO: Use vec2<float>
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

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << x << ", " << y;
			}
	};

	/// TODO: Use vec2<float>
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

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << xOffset << ", " << yOffset;
			}
	};
}