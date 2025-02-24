#pragma once

#include "Engine/Event/Event.hpp"

namespace ProjectA {
	class ApplicationEvent : public Event {
		protected:
			ApplicationEvent() = default;

		public:
			EVENT_CATEGORY(EventCategory::APPLICATION)
	};

	class ApplicationRenderEvent : public ApplicationEvent {
		public:
			ApplicationRenderEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_RENDER)
	};

	class ApplicationTickEvent : public ApplicationEvent {
		public:
			ApplicationTickEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_TICK)
	};

	class ApplicationUpdateEvent : public ApplicationEvent {
		public:
			ApplicationUpdateEvent() = default;

			EVENT_TYPE(EventType::APPLICATION_UPDATE)
	};
}