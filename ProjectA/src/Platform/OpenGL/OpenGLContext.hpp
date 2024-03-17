#pragma once

#include "ProjectA/Renderer/Context.hpp"

struct GLFWwindow;

namespace ProjectA {
	class OpenGLContext : public Context {
		private:
			GLFWwindow* window;

		public:
			OpenGLContext(GLFWwindow* window);

			void initialize() override;

			void swapBuffers() override;
	};
}