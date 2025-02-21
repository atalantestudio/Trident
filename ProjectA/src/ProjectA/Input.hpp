#pragma once

#include "ProjectA/base.hpp"
#include "ProjectA/KeyCode.hpp"
#include "ProjectA/MouseButton.hpp"

namespace ProjectA {
	class Input {
		public:
			static Scope<Input> create();

			static bool isKeyPressed(KeyCode keyCode);

			static bool isMouseButtonPressed(MouseButton button);

			static vec2<float32> getMousePosition();
	};
}