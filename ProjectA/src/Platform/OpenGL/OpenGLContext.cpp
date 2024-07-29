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
		LOG_CORE_INFO("{0} {1} {2}", (const char*) glGetString(GL_VENDOR), (const char*) glGetString(GL_RENDERER), (const char*) glGetString(GL_VERSION));
	}

	void OpenGLContext::swapBuffers() {
		glfwSwapBuffers(window);
	}
}