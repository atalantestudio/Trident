#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class WindowEvent : public Event {
		protected:
			WindowEvent() = default;

		public:
			EVENT_CATEGORY(EventCategory::WINDOW)
	};

	class WindowResizeEvent : public WindowEvent {
		private:
			// TODO: Use vec2<sint32>.
			uint32 width;
			uint32 height;

		public:
			WindowResizeEvent(uint32 width, uint32 height) :
				width(width),
				height(height)
			{}

			uint32 getWidth() const {
				return width;
			}

			uint32 getHeight() const {
				return height;
			}

			EVENT_TYPE(EventType::WINDOW_RESIZE)

			inline std::string toString() const override {
				std::stringstream stream;

				stream << getName() << ": " << width << ", " << height;

				return stream.str();
			}
	};
}