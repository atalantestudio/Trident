#include "pch.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Platform/OpenGL/OpenGLContext.hpp"

namespace ProjectA {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : window(window) {
		ENGINE_ASSERT(window, "Window handle is null");
	}

	void OpenGLContext::initialize() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwMakeContextCurrent(window);

		int gladLoadStatus = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

		ENGINE_ASSERT(gladLoadStatus, "Could not initialize GLAD");
		ENGINE_INFO("[], [], []", (const char*) glGetString(GL_VENDOR), (const char*) glGetString(GL_RENDERER), (const char*) glGetString(GL_VERSION));

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