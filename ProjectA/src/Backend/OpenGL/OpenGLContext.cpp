#include "pch.hpp"

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Backend/OpenGL/OpenGLContext.hpp"

namespace ProjectA {
	OpenGLContext::OpenGLContext(GLFWwindow* window) :
		window(window)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_ASSERT(window != nullptr, "Window handle is null.");

		glfwMakeContextCurrent(window);

		const int32 gladLoadStatus = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

		ENGINE_ASSERT(gladLoadStatus, "Failed to initialize GLAD.");

		/*GLint majorVersion;
		GLint minorVersion;

		glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
		glGetIntegerv(GL_MINOR_VERSION, &minorVersion);*/

		/*const GLubyte* gpuVendor = glGetString(GL_VENDOR);
		const GLubyte* gpuRenderer = glGetString(GL_RENDERER);
		const GLubyte* gpuVersion = glGetString(GL_VERSION);

		ENGINE_INFO("GPU: [] []", gpuVendor, gpuRenderer);
		ENGINE_INFO("GPU version: []", gpuVersion);*/
	}

	void OpenGLContext::swapBuffers() const {
		PROFILE_SCOPE(FUNC_SIG);

		glfwSwapBuffers(window);
	}
}