#pragma once

namespace ProjectA {
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