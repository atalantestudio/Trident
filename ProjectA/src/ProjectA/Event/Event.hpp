#pragma once

#include "ProjectA/core.hpp"
#include <functional>

namespace ProjectA {
	enum class EventType {
		None = 0,
		ApplicationRender,
		ApplicationTick,
		ApplicationUpdate,
		KeyPressed,
		KeyReleased,
		MouseMoved,
		MouseLeftPressed,
		MouseLeftReleased,
		MouseRightPressed,
		MouseRightReleased,
		MouseScrolled,
		WindowClose,
		WindowFocus,
		WindowFocusLost,
		WindowMoved,
		WindowResize,
	};

	enum EventCategory {
		None = 0,
		Input = BIT_SHIFT_ID(0),
		Keyboard = BIT_SHIFT_ID(1),
		Mouse = BIT_SHIFT_ID(2),
		MouseButton = BIT_SHIFT_ID(3),
	};

	class API Event {
		friend class EventDispatcher;

		protected:
			bool handled = false;

		public:
			// Pure virtual functions (must be implemented by the child classes)
			// virtual const char* getName() const = 0;
			virtual EventType getType() const = 0;
			virtual int getCategories() const = 0;

			inline bool hasCategory(EventCategory category) const {
				return getCategories() & category;
			}
	};

	class EventDispatcher {
		template<typename T>
		using EventFunction = std::function<bool (T&)>;

		private:
			Event& event;

		public:
			EventDispatcher(Event& event) : event(event) {}

			template<typename T>
			bool dispatch(EventFunction<T> function) {
				if (event.getType() != T::getType()) return false;

				event.handled = function(*((T*) &event));

				return true;
			}
	};
}