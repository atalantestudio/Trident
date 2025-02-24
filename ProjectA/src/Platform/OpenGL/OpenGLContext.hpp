#pragma once

#include "Engine/Renderer/Context.hpp"

struct GLFWwindow;

namespace ProjectA {
	class OpenGLContext : public Context {
		public:
			OpenGLContext(GLFWwindow* window);

			void swapBuffers() const override;

		private:
			GLFWwindow* window;
	};
}