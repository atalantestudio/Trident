#pragma once

#include "Engine/API/Event/WindowEvent.hpp"

namespace ProjectA {
	class WindowCloseEvent : public WindowEvent {
		public:
			explicit WindowCloseEvent() = default;

			EVENT_TYPE(EventType::WINDOW_CLOSE)
	};
}