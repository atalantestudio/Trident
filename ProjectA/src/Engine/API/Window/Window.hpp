#pragma once

namespace ProjectA {
	struct WindowDescriptor {
		std::string name;

		// TODO: Use vec2<sint32>.
		uint32 width;
		uint32 height;
	};

	template<typename Event>
	using EventListener = std::function<void(Event& event)>;

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

			template<typename Event>
			void setEventListener(EventListener<Event> eventListener) const;

		public:
			void* data;
	};
}