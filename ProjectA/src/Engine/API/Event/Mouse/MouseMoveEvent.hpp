#pragma once

#include "Engine/API/Event/Event.hpp"

namespace ProjectA {
	class MouseMoveEvent : public Event {
		public:
			// TODO: Use vec2<sint32>.
			MouseMoveEvent(sint32 x, sint32 y);

			EVENT_TYPE(EventType::MOUSE_MOVE)
			EVENT_CATEGORY(EventCategory::INPUT | EventCategory::MOUSE)

			std::string toString() const override;

		private:
			// TODO: Use vec2<sint32>.
			const sint32 x;
			const sint32 y;
	};
}