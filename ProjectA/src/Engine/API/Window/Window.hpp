#pragma once

#include "Engine/API/Window/Descriptor/WindowDescriptor.hpp"

namespace ProjectA {
	class Window {
		public:
			explicit Window(const WindowDescriptor& windowDescriptor);

			~Window();

			// Returns the position of the upper-left corner of the client area.
			const sint32* getPosition() const;

			// Returns the size of the client area.
			const sint32* getSize() const;

			float32 getAspectRatio() const {
				const sint32* size = getSize();

				return static_cast<float32>(size[0]) / size[1];
			}

			// Returns whether the window is maximized.
			bool isMaximized() const;

			// Returns whether the window is minimized.
			bool isMinimized() const;

			bool isCursorCaptured() const;

			// Returns the position of the mouse, relative to the top-left corner of the client area.
			const sint32* getMousePosition() const;

			void show() const;

			void captureCursor() const;

			void releaseCursor() const;

			// Peeks, translates and dispatches incoming non-queued messages.
			void processMessages() const;

			void waitMessage() const;

		public:
			void* data;
	};
}