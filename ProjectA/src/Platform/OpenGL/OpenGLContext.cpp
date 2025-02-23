#include "pch.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Platform/OpenGL/OpenGLContext.hpp"

namespace ProjectA {
	OpenGLContext::OpenGLContext(GLFWwindow* window) : window(window) {
		ENGINE_ASSERT(window, "Window handle is null");
	}

	void OpenGLContext::initialize() {
		PROFILE_SCOPE(__FUNCSIG__);

		glfwMakeContextCurrent(window);

		const int32 gladLoadStatus = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

		ENGINE_ASSERT(gladLoadStatus, "Could not initialize GLAD");

		const GLubyte* gpuVendor = glGetString(GL_VENDOR);
		const GLubyte* gpuRenderer = glGetString(GL_RENDERER);
		const GLubyte* gpuVersion = glGetString(GL_VERSION);

		ENGINE_INFO("GPU: [] []", gpuVendor, gpuRenderer, gpuVersion);
		ENGINE_INFO("GPU version: []", gpuVersion);

		/* #ifdef ENABLE_ASSERTS
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