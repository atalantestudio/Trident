#pragma once

#include "Engine/API/Event/EventListener.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

namespace ProjectA {
	struct WindowDescriptor {
		std::string name;

		// TODO: Use vec2<sint32/uint32> (scalar).
		uint32 width;
		uint32 height;

		EventListener<MouseDownEvent> mouseDownEventListener;
		EventListener<MouseMoveEvent> mouseMoveEventListener;
		EventListener<MouseUpEvent> mouseUpEventListener;
		EventListener<MouseWheelEvent> mouseWheelEventListener;
		EventListener<WindowCloseEvent> windowCloseEventListener;
		EventListener<WindowResizeEvent> windowResizeEventListener;
	};
}