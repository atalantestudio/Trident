#pragma once

#include "Engine/API/Event/EventListener.hpp"
#include "Engine/API/Event/KeyEvent.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

namespace ProjectA {
	struct WindowDescriptor {
		std::string name;

		Extent2D size;

		EventListener<KeyDownEvent> keyDownEventListener;
		EventListener<KeyUpEvent> keyUpEventListener;
		EventListener<MouseDownEvent> mouseDownEventListener;
		EventListener<MouseMoveEvent> mouseMoveEventListener;
		EventListener<MouseUpEvent> mouseUpEventListener;
		EventListener<MouseWheelEvent> mouseWheelEventListener;
		EventListener<WindowCloseEvent> windowCloseEventListener;
		EventListener<WindowResizeEvent> windowResizeEventListener;
	};
}