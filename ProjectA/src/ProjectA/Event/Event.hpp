#pragma once

#include "pch.hpp"
#include "ProjectA/core.hpp"

namespace ProjectA {
	enum class EventType {
		EventTypeNone = 0,
		EventTypeApplicationRender,
		EventTypeApplicationTick,
		EventTypeApplicationUpdate,
		EventTypeKeyPress,
		EventTypeKeyRelease,
		EventTypeKeyType,
		EventTypeMouseButtonPress,
		EventTypeMouseButtonRelease,
		EventTypeMouseMove,
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

		public:
			bool handled = false;

			virtual EventType getType() const = 0;
			virtual int getCategories() const = 0;
			virtual const char* getName() const = 0;

			inline bool hasCategory(EventCategory category) const {
				return getCategories() & category;
			}

			virtual inline std::ostream& operator <<(std::ostream& stream) const {
				return stream << getName();
			}
	};

	class EventDispatcher {
		private:
			Event& event;

		public:
			EventDispatcher(Event& event) : event(event) {}

			template<typename T, typename F>
			bool dispatch(const F& function) {
				if (event.getType() != T::getStaticType()) {
					return false;
				}

				event.handled = function(static_cast<T&>(event));

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