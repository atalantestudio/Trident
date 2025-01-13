#pragma once

#include "ProjectA/Event/Event.hpp"

namespace ProjectA {
	class ApplicationEvent : public Event {
		protected:
			ApplicationEvent() = default;

		public:
			EVENT_CATEGORY(EventCategoryApplication)
	};

	class ApplicationRenderEvent : public ApplicationEvent {
		public:
			ApplicationRenderEvent() = default;

			EVENT_TYPE(EventTypeApplicationRender)
	};

	class ApplicationTickEvent : public ApplicationEvent {
		public:
			ApplicationTickEvent() = default;

			EVENT_TYPE(EventTypeApplicationTick)
	};

	class ApplicationUpdateEvent : public ApplicationEvent {
		public:
			ApplicationUpdateEvent() = default;

			EVENT_TYPE(EventTypeApplicationUpdate)
	};
}