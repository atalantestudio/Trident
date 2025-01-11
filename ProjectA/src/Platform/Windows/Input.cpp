#include "pch.hpp"

#include <GLFW/glfw3.h>

#include "Platform/Windows/Input.hpp"
#include "ProjectA/Application.hpp"

namespace ProjectA {
	InputInterface* InputInterface::instance = new Input();

	bool Input::isKeyPressedImpl(int keyCode) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int keyState = glfwGetKey(window, keyCode);

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressedImpl(int button) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int buttonState = glfwGetMouseButton(window, button);

		return buttonState == GLFW_PRESS;
	}

	vec2<float> Input::getMousePositionImpl() {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x, y;

		glfwGetCursorPos(window, &x, &y);

		return {(float) x, (float) y};
	}
}