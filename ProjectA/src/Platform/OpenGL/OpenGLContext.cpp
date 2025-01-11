#include "pch.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLContext.hpp"

namespace ProjectA {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : window(window) {
		CORE_ASSERT(window, "Window handle is null");
	}

	void OpenGLContext::initialize() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwMakeContextCurrent(window);

		int gladLoadStatus = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

		CORE_ASSERT(gladLoadStatus, "Could not initialize GLAD");
		LOG_CORE_INFO("{0} {1} {2}", (const char*) glGetString(GL_VENDOR), (const char*) glGetString(GL_RENDERER), (const char*) glGetString(GL_VERSION));

		/* #ifdef ASSERTS
			GLint majorVersion;
			GLint minorVersion;

			glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
			glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
		#endif */
	}

	void OpenGLContext::swapBuffers() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwSwapBuffers(window);
	}
}