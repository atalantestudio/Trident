#include "pch.hpp"

#include <glfw/glfw3.h>

#include "Engine/Application.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	bool Input::isKeyPressed(const KeyCode keyCode) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		const sint32 keyState = glfwGetKey(window, static_cast<sint32>(keyCode));

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(const MouseButton button) {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		const sint32 buttonState = glfwGetMouseButton(window, static_cast<sint32>(button));

		return buttonState == GLFW_PRESS;
	}

	vec2<float64> Input::getMousePosition() {
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWindow());

		float64 x;
		float64 y;

		glfwGetCursorPos(window, &x, &y);

		return {x, y};
	}
}