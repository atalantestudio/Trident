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

	//typedef *(void)() EventListener;

	class Window {
		public:
			Window(const WindowDescriptor& windowDescriptor);

			~Window();

			// Returns the position of the upper-left corner of the client area.
			const sint32* getPosition() const;

			// Returns the size of the client area.
			const sint32* getSize() const;

			// Returns whether the window is maximized.
			bool isMaximized() const;

			// Returns whether the window is minimized.
			bool isMinimized() const;

			// Returns the position of the mouse, relative to the top-left corner of the client area.
			const sint32* getMousePosition() const;

			// Peeks, translates and dispatches incoming non-queued messages.
			void processMessages() const;

			//template<typename T>
			//void setEventListener();

			// Shows an error message box, then exits.
			// Differs from Platform::fatalExit in that the message box is owned by this window.
			void fatalExit(uint32 code, const std::string& message) const;

		public:
			void* data;
	};
}