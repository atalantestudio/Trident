#pragma once

#include "Engine/API/Event/Event.hpp"
#include "Engine/base.hpp"

namespace ProjectA {
	struct WindowOldDescriptor {
		// TODO: Replace with const char*?
		std::string name;

		// TODO: Use vec2<sint32>.
		// TODO: Determine adapted type for width and height.
		uint32 width;
		uint32 height;
	};

	class WindowOld {
		public:
			using EventCallback = std::function<void(Event& event)>;

		public:
			//static scope<WindowOld> create(const WindowOldDescriptor& descriptor);

		public:
			virtual ~WindowOld() = default;

			virtual void* getNativeWindow() const = 0;

			// TODO: Use vec2<sint32>.
			virtual uint32 getWidth() const = 0;
			virtual uint32 getHeight() const = 0;

			virtual void setEventCallback(const EventCallback& callback) = 0;
			virtual void update() = 0;
	};
}