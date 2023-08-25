#pragma once

#include "ProjectA/core.hpp"
#include <functional>
#include <string>

namespace ProjectA {
	enum class EventType {
		None = 0,
		ApplicationRender,
		ApplicationTick,
		ApplicationUpdate,
		KeyPressed,
		KeyReleased,
		MouseMoved,
		MouseButtonPressed,
		MouseButtonReleased,
		MouseScrolled,
		WindowClosed,
		WindowFocused,
		WindowFocusLost,
		WindowMoved,
		WindowResized,
	};

	enum EventCategory {
		None = 0,
		_Application = BIT_SHIFT_ID(0),
		Input = BIT_SHIFT_ID(1),
		Keyboard = BIT_SHIFT_ID(2),
		Mouse = BIT_SHIFT_ID(3),
		MouseButton = BIT_SHIFT_ID(4),
		Window = BIT_SHIFT_ID(5),
	};

	class API Event {
		friend class EventDispatcher;

		protected:
			bool handled = false;

		public:
			virtual EventType getType() const = 0;
			virtual int getCategories() const = 0;
			virtual const char* getName() const = 0;
			virtual std::string toString() const {
				return getName();
			}

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

				event.handled = function(*reinterpret_cast<T*>(&event));

				return true;
			}
	};

	#define EVENT_CATEGORY(category) \
		virtual int getCategories() const override { \
			return category; \
		}

	#define EVENT_TYPE(type) \
		static EventType getStaticType() { \
			return EventType::##type; \
		} \
		\
		virtual EventType getType() const override { \
			return getStaticType(); \
		} \
		\
		virtual const char* getName() const override { \
			return #type; \
		}
}