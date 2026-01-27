#pragma once

#include "Engine/API/Event/EventListener.hpp"
#include "Engine/API/Event/Mouse/MouseDownEvent.hpp"
#include "Engine/API/Event/Mouse/MouseMoveEvent.hpp"
#include "Engine/API/Event/Mouse/MouseUpEvent.hpp"

namespace Trident {
	static constexpr void OnMouseDownDefaultListener(MouseDownEvent& event) {}
	static constexpr void OnMouseUpDefaultListener(MouseUpEvent& event) {}
	static constexpr void OnMouseMoveDefaultListener(MouseMoveEvent& event) {}

	struct EventListeners {
		EventListener<MouseDownEvent> onMouseDown = OnMouseDownDefaultListener;
		EventListener<MouseUpEvent> onMouseUp = OnMouseUpDefaultListener;
		EventListener<MouseMoveEvent> onMouseHover = OnMouseMoveDefaultListener;
		EventListener<MouseMoveEvent> onMouseLeave = OnMouseMoveDefaultListener;
		EventListener<MouseUpEvent> onMouseClick = OnMouseUpDefaultListener;
	};
}