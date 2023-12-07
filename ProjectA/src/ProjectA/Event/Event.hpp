#pragma once

#include "ProjectA/core.hpp"
#include <functional>
#include <string>

namespace ProjectA {
	enum class EventType {
		EventTypeNone = 0,
		EventTypeApplicationRender,
		EventTypeApplicationTick,
		EventTypeApplicationUpdate,
		EventTypeKeyPress,
		EventTypeKeyRelease,
		EventTypeMouseMove,
		EventTypeMouseButtonPress,
		EventTypeMouseButtonRelease,
		EventTypeMouseScroll,
		EventTypeWindowClose,
		EventTypeWindowResize,
	};

	enum EventCategory {
		EventCategoryNone = 0,
		EventCategoryApplication = BIT_SHIFT_ID(0),
		EventCategoryInput = BIT_SHIFT_ID(1),
		EventCategoryKeyboard = BIT_SHIFT_ID(2),
		EventCategoryMouse = BIT_SHIFT_ID(3),
		EventCategoryMouseButton = BIT_SHIFT_ID(4),
		EventCategoryWindow = BIT_SHIFT_ID(5),
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
				if (event.getType() != T::getStaticType()) {
					return false;
				}

				event.handled = function(*reinterpret_cast<T*>(&event));

				return true;
			}
	};

	inline std::ostream& operator << (std::ostream& stream, const Event& event) {
		return stream << event.toString();
	}

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