#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Engine/Application.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	bool Input::isKeyPressed(KeyCode keyCode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		const int32 keyState = glfwGetKey(window, static_cast<int32>(keyCode));

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(MouseButton button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		const int32 buttonState = glfwGetMouseButton(window, static_cast<int32>(button));

		return buttonState == GLFW_PRESS;
	}

	vec2<float32> Input::getMousePosition() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		float64 x;
		float64 y;

		glfwGetCursorPos(window, &x, &y);

		return {static_cast<float32>(x), static_cast<float32>(y)};
	}
}