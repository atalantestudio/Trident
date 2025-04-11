#pragma once

#include "Engine/API/Window/WindowClass.hpp"

namespace ProjectA {
	struct WindowDescriptor {
		// The window name.
		const char* name;

		// TODO: Use vec2<sint32>.
		uint32 x = 0;
		uint32 y = 0;

		// TODO: Use vec2<sint32>.
		uint32 width;
		uint32 height;
	};

	class Window {
		public:
			Window(const WindowClass& windowClass, const WindowDescriptor& windowDescriptor);

			~Window();

			// Returns the position of the mouse, relative to the top-left corner of the client area.
			const sint32* getMousePosition() const;

			// Peeks, translates and dispatchs incoming non-queued messages.
			void dispatchMessages() const;

		public:
			const void* data;
	};
}