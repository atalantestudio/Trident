// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Event/EventListener.hpp"
#include "Engine/API/Event/Keyboard/KeyDownEvent.hpp"
#include "Engine/API/Event/Keyboard/KeyUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"
#include "Engine/API/Event/Mouse/MouseWheelEvent.hpp"
#include "Engine/API/Event/Window/WindowCloseEvent.hpp"
#include "Engine/API/Event/Window/WindowResizeEvent.hpp"

namespace Trident {
	struct WindowDescriptor {
		view<char8> name;
		Extent2D extent;
		bool maximized;
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