#pragma once

#include "ProjectA/Event/Event.hpp"

namespace ProjectA {
	class WindowEvent : public Event {
		protected:
			WindowEvent() = default;

		public:
			EVENT_CATEGORY(EventCategoryWindow)
	};

	class WindowCloseEvent : public WindowEvent {
		public:
			WindowCloseEvent() = default;

			EVENT_TYPE(EventTypeWindowClose)
	};

	class WindowResizeEvent : public WindowEvent {
		private:
			// TODO: Replace with vec2<int32> (or better, vec2<uint32>).
			uint32 width;
			uint32 height;

		public:
			WindowResizeEvent(uint32 width, uint32 height) : width(width), height(height) {}

			uint32 getWidth() const {
				return width;
			}

			uint32 getHeight() const {
				return height;
			}

			EVENT_TYPE(EventTypeWindowResize)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << width << ", " << height;

				return stream.str();
			}
	};
}