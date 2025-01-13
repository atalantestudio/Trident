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
		EventCategoryApplication = 1 << 0,
		EventCategoryInput = 1 << 1,
		EventCategoryKeyboard = 1 << 2,
		EventCategoryMouse = 1 << 3,
		EventCategoryMouseButton = 1 << 4,
		EventCategoryWindow = 1 << 5,
	};

	class Event {
		friend class EventDispatcher;

		private:
			virtual std::string toString() const {
				return getName();
			}

		public:
			bool handled = false;

			virtual EventType getType() const = 0;
			virtual int32 getCategories() const = 0;
			virtual const char* getName() const = 0;

			inline bool hasCategory(EventCategory category) const {
				return getCategories() & category;
			}

			friend std::ostream& operator <<(std::ostream& stream, const Event& event) {
				return stream << event.toString();
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
		virtual int32 getCategories() const override { \
			return category; \
		}

	#define EVENT_TYPE(type) \
		static EventType getStaticType() { \
			return EventType::type; \
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