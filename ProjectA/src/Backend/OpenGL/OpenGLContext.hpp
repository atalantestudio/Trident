#pragma once

#include <glfw/glfw3.h>

#include "Engine/Renderer/Context.hpp"

namespace ProjectA {
	class OpenGLContext : public Context {
		public:
			OpenGLContext(GLFWwindow* window);

		private:
			GLFWwindow* window;
	};
}