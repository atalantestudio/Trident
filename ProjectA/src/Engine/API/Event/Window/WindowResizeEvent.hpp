#pragma once

#include "Engine/API/Event/WindowEvent.hpp"

namespace ProjectA {
	class WindowResizeEvent : public WindowEvent {
		private:
			// TODO: Use vec2<sint32>.
			sint32 width;
			sint32 height;

		public:
			WindowResizeEvent(sint32 width, sint32 height) :
				width(width),
				height(height)
			{}

			sint32 getWidth() const {
				return width;
			}

			sint32 getHeight() const {
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