// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Window/Descriptor/WindowDescriptor.hpp"
#include "Engine/API/Window/PointerType.hpp"

namespace Trident {
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

			bool isKeyDown(KeyCode keyCode) const;

			bool isMaximized() const;

			bool isMinimized() const;

			// Returns the position of the mouse, relative to the top-left corner of the client area.
			const sint32* getMousePosition() const;

			vec2<float32> getNdcMousePosition() const {
				const sint32* size = getSize();
				const sint32* mousePosition = getMousePosition();

				return {
					-1.0f + static_cast<float32>(mousePosition[0]) / (size[0] * 0.5f),
					 1.0f - static_cast<float32>(mousePosition[1]) / (size[1] * 0.5f),
				};
			}

			bool isPointerLocked() const;

			void show() const;

			// Hides the pointer and captures it into the client area.
			// While in this state, mouse move events store the mouse delta instead of a relative position.
			void lockPointer() const;

			// Shows the pointer and releases it from the client area.
			void unlockPointer() const;

			// Peeks, translates and dispatches incoming non-queued messages.
			void processMessages() const;

			void waitMessage() const;

			void updatePointerType(PointerType pointerType) const;

		// TODO: Make data and key map private?
		public:
			void* data;
	};
}