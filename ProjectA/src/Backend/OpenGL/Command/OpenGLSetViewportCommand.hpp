#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetViewportCommand final : public OpenGLCommand {
		public:
			explicit OpenGLSetViewportCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
				x(x),
				y(y),
				width(width),
				height(height)
			{}

			void execute() const final override {
				glViewport(x, y, width, height);
			}

		private:
			const GLint x;
			const GLint y;
			const GLsizei width;
			const GLsizei height;
	};
}