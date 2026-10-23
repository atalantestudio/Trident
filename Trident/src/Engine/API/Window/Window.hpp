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

			/// Returns the position of the top-left corner of the client area.
			const sint32* getPosition() const;

			Extent2D getExtent() const;

			inline float32 getAspectRatio() const {
				const Extent2D extent = getExtent();

				return static_cast<float32>(extent.x) / extent.y;
			}

			/// Returns the dots per inch value for the window.
			uint32 getDpi() const;

			bool isKeyDown(KeyCode keyCode) const;

			bool isMaximized() const;

			bool isMinimized() const;

			/// Returns the position of the mouse relative to the top-left corner of the client area.
			vec2<sint32> getMousePosition() const;

			inline vec2<float32> getNdcMousePosition() const {
				const Extent2D extent = getExtent();
				const vec2<float32> mousePosition = vec2<float32>(getMousePosition());
				const vec2<float32> ndcMousePosition = mousePosition / (vec2<float32>(extent) * 0.5f);

				return {ndcMousePosition.x - 1.0f, -ndcMousePosition.y + 1.0f};
			}

			bool isPointerLocked() const;

			void show() const;

			/// Hides the pointer and captures it into the client area.
			/// While in this state, mouse move events store the mouse delta instead of a relative position.
			void lockPointer() const;

			/// Shows the pointer and releases it from the client area.
			void unlockPointer() const;

			/// Peeks, translates and dispatches incoming non-queued messages.
			void processMessages() const;

			void waitMessage() const;

			void updatePointerType(PointerType pointerType) const;

			/// Copies `text` to the clipboard.
			void copyToClipboard(
				/// A non-terminated UTF-8 string.
				view<char8> text
			) const;

		// TODO: Make private?
		public:
			void* data;
	};
}