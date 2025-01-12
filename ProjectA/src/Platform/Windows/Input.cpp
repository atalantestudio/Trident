#include "pch.hpp"

#include "Platform/Windows/Input.hpp"

namespace ProjectA {
	InputInterface* InputInterface::instance = new Input();

	bool Input::isKeyPressedImpl(int keyCode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int keyState = glfwGetKey(window, keyCode);

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressedImpl(int button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		int buttonState = glfwGetMouseButton(window, button);

		return buttonState == GLFW_PRESS;
	}

	vec2<float> Input::getMousePositionImpl() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x;
		double y;

		glfwGetCursorPos(window, &x, &y);

		return {(float) x, (float) y};
	}
}