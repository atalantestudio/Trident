#pragma once

#include "Engine/base.hpp"
#include "Engine/KeyCode.hpp"
#include "Engine/MouseButton.hpp"

namespace ProjectA {
	class Input {
		public:
			static bool isKeyPressed(KeyCode keyCode);

			static bool isMouseButtonPressed(MouseButton button);

			static vec2<float64> getMousePosition();
	};
}