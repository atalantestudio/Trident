#pragma once

#include <sstream>
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

	class WindowResizeEvent : public WindowEvent {
		private:
			int width, height;

		public:
			WindowResizeEvent(int width, int height) : width(width), height(height) {}

			EVENT_TYPE(EventTypeWindowResize)

			std::string toString() const {
				std::stringstream stream;
				stream << getName() << ": " << width << ", " << height;

				return stream.str();
			}
	};
}