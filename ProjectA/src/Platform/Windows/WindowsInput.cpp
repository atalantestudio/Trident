#include "pch.hpp"

#include "Platform/Windows/WindowsInput.hpp"

namespace ProjectA {
	bool WindowsInput::isKeyPressedImpl(KeyCode keyCode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		const int keyState = glfwGetKey(window, static_cast<int32>(keyCode));

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool WindowsInput::isMouseButtonPressedImpl(MouseButton button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		const int buttonState = glfwGetMouseButton(window, static_cast<int32>(button));

		return buttonState == GLFW_PRESS;
	}

	lyah::vec2<float> WindowsInput::getMousePositionImpl() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());
		double x;
		double y;

		glfwGetCursorPos(window, &x, &y);

		return {(float) x, (float) y};
	}
}