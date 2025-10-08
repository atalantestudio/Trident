#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class ApplicationEvent : public Event {
		protected:
			explicit ApplicationEvent() = default;

		public:
			EVENT_CATEGORY(EventCategory::APPLICATION)
	};

	class ApplicationRenderEvent : public ApplicationEvent {
		public:
			explicit ApplicationRenderEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_RENDER)
	};

	class ApplicationTickEvent : public ApplicationEvent {
		public:
			explicit ApplicationTickEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_TICK)
	};

	class ApplicationUpdateEvent : public ApplicationEvent {
		public:
			explicit ApplicationUpdateEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_UPDATE)
	};
}