#pragma once

#include "pch.hpp"

namespace ProjectA {
	enum class EventType : uint32 {
		NONE = 0,
		APPLICATION_RENDER,
		APPLICATION_TICK,
		APPLICATION_UPDATE,
		KEY_PRESS,
		KEY_RELEASE,
		KEY_TYPE,
		BUTTON_PRESS,
		BUTTON_RELEASE,
		MOUSE_MOVE,
		MOUSE_SCROLL,
		WINDOW_CLOSE,
		WINDOW_RESIZE,
	};

	enum class EventCategory : uint8 {
		NONE = 0,
		APPLICATION = BIT(0),
		INPUT = BIT(1),
		KEYBOARD = BIT(2),
		MOUSE = BIT(3),
		MOUSE_BUTTON = BIT(4),
		WINDOW = BIT(5),
	};

	AS_FLAG(EventCategory);

	class Event {
		friend class EventDispatcher;

		public:
			bool handled = false;

		public:
			virtual ~Event() = default;

			virtual EventType getType() const = 0;
			virtual uint32 getCategories() const = 0;
			virtual const char* getName() const = 0;

			inline bool hasCategory(EventCategory category) const {
				return getCategories() & category;
			}

			friend std::ostream& operator <<(std::ostream& stream, const Event& event) {
				return stream << event.toString();
			}

		private:
			virtual std::string toString() const {
				return getName();
			}
	};

	class EventDispatcher {
		public:
			EventDispatcher(Event& event) :
				event(event)
			{}

			template<typename T, typename F>
			bool dispatch(const F& function) {
				if (event.getType() != T::getStaticType()) {
					return false;
				}

				event.handled = function(static_cast<T&>(event));

				return true;
			}

		private:
			Event& event;
	};

	#define EVENT_CATEGORY(category) \
		virtual uint32 getCategories() const override { \
			return static_cast<uint32>(category); \
		}

	#define EVENT_TYPE(type) \
		static EventType getStaticType() { \
			return type; \
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