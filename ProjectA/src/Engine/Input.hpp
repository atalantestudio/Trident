#pragma once

#include "Engine/base.hpp"
#include "Engine/KeyCode.hpp"
#include "Engine/MouseButton.hpp"

namespace ProjectA {
	class Input {
		public:
			static scope<Input> create();

			static bool isKeyPressed(const KeyCode keyCode);

			static bool isMouseButtonPressed(const MouseButton button);

			static vec2<float64> getMousePosition();
	};
}