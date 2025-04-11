#pragma once

#include <glfw/glfw3.h>

namespace ProjectA {
	class OpenGLContext {
		public:
			OpenGLContext(GLFWwindow* window);

		private:
			GLFWwindow* window;
	};
}