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

	class WindowClosedEvent : public WindowEvent {
		public:
			WindowClosedEvent() {}

			EVENT_TYPE(EventTypeWindowClosed)
	};

	class WindowFocusedEvent : public WindowEvent {
		public:
			WindowFocusedEvent() {}

			EVENT_TYPE(EventTypeWindowFocused)
	};

	class WindowFocusLostEvent : public WindowEvent {
		public:
			WindowFocusLostEvent() {}

			EVENT_TYPE(EventTypeWindowFocusLost)
	};

	class WindowMovedEvent : public WindowEvent {
		public:
			WindowMovedEvent() {}

			EVENT_TYPE(EventTypeWindowMoved)
	};

	class WindowResizedEvent : public WindowEvent {
		private:
			int width, height;

		public:
			WindowResizedEvent(int width, int height) : width(width), height(height) {}

			EVENT_TYPE(EventTypeWindowResized)

			std::string toString() const {
				std::stringstream stream;
				stream << getName() << ": " << width << ", " << height;

				return stream.str();
			}
	};
}