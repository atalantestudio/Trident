#include "pch.hpp"
#include "OpenGLContext.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace ProjectA {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : window(window) {
		CORE_ASSERT(window, "Window handle is null");
	}

	void OpenGLContext::initialize() {
		glfwMakeContextCurrent(window);

		int gladLoadStatus = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

		CORE_ASSERT(gladLoadStatus, "Could not initialize GLAD");
	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(window);
	}
}