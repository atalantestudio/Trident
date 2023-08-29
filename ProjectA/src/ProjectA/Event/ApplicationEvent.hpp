#pragma once

#include "Event.hpp"

namespace ProjectA {
	class ApplicationEvent : public Event {
		protected:
			ApplicationEvent() {}

		public:
			EVENT_CATEGORY(EventCategoryApplication)
	};

	class ApplicationRenderEvent : public ApplicationEvent {
		public:
			ApplicationRenderEvent() {}

			EVENT_TYPE(EventTypeApplicationRender)
	};

	class ApplicationTickEvent : public ApplicationEvent {
		public:
			ApplicationTickEvent() {}

			EVENT_TYPE(EventTypeApplicationTick)
	};

	class ApplicationUpdateEvent : public ApplicationEvent {
		public:
			ApplicationUpdateEvent() {}

			EVENT_TYPE(EventTypeApplicationUpdate)
	};
}