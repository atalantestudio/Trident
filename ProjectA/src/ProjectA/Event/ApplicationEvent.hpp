#pragma once

#include "Event.hpp"

namespace ProjectA {
	class ApplicationEvent : public Event {
		protected:
			ApplicationEvent() {}

		public:
			EVENT_CATEGORY(EventCategory::_Application)
	};

	class ApplicationRenderEvent : public ApplicationEvent {
		public:
			ApplicationRenderEvent() {}

			EVENT_TYPE(ApplicationRender)
	};

	class ApplicationTickEvent : public ApplicationEvent {
		public:
			ApplicationTickEvent() {}

			EVENT_TYPE(ApplicationTick)
	};

	class ApplicationUpdateEvent : public ApplicationEvent {
		public:
			ApplicationUpdateEvent() {}

			EVENT_TYPE(ApplicationUpdate)
	};
}