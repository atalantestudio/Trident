#pragma once

#include "Engine/API/Event/WindowEvent.hpp"

namespace ProjectA {
	class WindowCloseEvent : public WindowEvent {
		public:
			WindowCloseEvent() = default;

			EVENT_TYPE(EventType::WINDOW_CLOSE)
	};
}