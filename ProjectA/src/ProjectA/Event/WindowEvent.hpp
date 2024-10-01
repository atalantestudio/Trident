#pragma once

#include "Event.hpp"

namespace ProjectA {
	class WindowEvent : public Event {
		protected:
			WindowEvent() {}

		public:
			EVENT_CATEGORY(EventCategoryWindow)
	};

	class WindowCloseEvent : public WindowEvent {
		public:
			WindowCloseEvent() {}

			EVENT_TYPE(EventTypeWindowClose)
	};

	/// TODO: Create vec2<int> for this case?
	class WindowResizeEvent : public WindowEvent {
		private:
			int width, height;

		public:
			WindowResizeEvent(int width, int height) : width(width), height(height) {}

			inline int getWidth() const {
				return width;
			}

			inline int getHeight() const {
				return height;
			}

			EVENT_TYPE(EventTypeWindowResize)

			inline std::ostream& operator <<(std::ostream& stream) const override {
				return stream << getName() << ": " << width << ", " << height;
			}
	};
}