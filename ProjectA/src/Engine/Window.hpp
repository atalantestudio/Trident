#pragma once

#include "Engine/base.hpp"
#include "Engine/Event/Event.hpp"

namespace ProjectA {
	struct WindowDescriptor {
		// TODO: Replace with const char*?
		std::string name;

		// TODO: Use vec2<sint32>.
		// TODO: Determine adapted type for width and height.
		uint32 width;
		uint32 height;
	};

	class Window {
		public:
			using EventCallback = std::function<void(Event&)>;

		public:
			static scope<Window> create(const WindowDescriptor& descriptor);

		public:
			virtual ~Window() = default;

			virtual void* getNativeWindow() const = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual void setEventCallback(const EventCallback& callback) = 0;
			virtual void update() = 0;
	};
}