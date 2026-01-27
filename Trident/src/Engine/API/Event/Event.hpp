#pragma once

#include "Engine/API/Event/EventCategory.hpp"
#include "Engine/API/Event/EventType.hpp"

namespace Trident {
	class Event {
		friend class EventDispatcher;

		public:
			virtual ~Event() = default;

			virtual EventType getType() const = 0;

			virtual uint32 getCategories() const = 0;

			virtual const char8* getName() const = 0;

			inline bool hasCategory(EventCategory category) const {
				const uint32 categories = getCategories();

				return categories & category;
			}

		public:
			bool handled = false;
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
		virtual const char8* getName() const override { \
			return #type; \
		}
}