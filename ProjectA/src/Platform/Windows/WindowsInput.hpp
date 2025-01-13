#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "ProjectA/Application.hpp"
#include "ProjectA/Input.hpp"

namespace ProjectA {
	class WindowsInput : public Input {
		protected:
			bool isKeyPressedImpl(KeyCode keyCode) final override;

			bool isMouseButtonPressedImpl(MouseButton button) final override;

			vec2<float> getMousePositionImpl() final override;
	};
}