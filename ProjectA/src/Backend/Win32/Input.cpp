#include "pch.hpp"

#include "Engine/Application.hpp"
#include "Engine/Input.hpp"

namespace ProjectA {
	bool Input::isKeyPressed(KeyCode keyCode) {
		/*GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindowOld().getNativeWindow());

		const sint32 keyState = glfwGetKey(window, static_cast<sint32>(keyCode));

		return keyState == GLFW_PRESS || keyState == GLFW_REPEAT;*/
		return false;
	}

	bool Input::isMouseButtonPressed(MouseButton button) {
		/*GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindowOld().getNativeWindow());

		const sint32 buttonState = glfwGetMouseButton(window, static_cast<sint32>(button));

		return buttonState == GLFW_PRESS;*/
		return false;
	}

	vec2<float64> Input::getMousePosition() {
		/*GLFWwindow* window = static_cast<GLFWwindow*>(Application::get().getWindowOld().getNativeWindow());

		float64 x;
		float64 y;

		glfwGetCursorPos(window, &x, &y);

		return {x, y};*/
		return {0.0, 0.0};
	}
}