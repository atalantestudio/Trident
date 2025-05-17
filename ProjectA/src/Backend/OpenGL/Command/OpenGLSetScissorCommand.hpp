#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetScissorCommand : public OpenGLCommand {
		public:
			OpenGLSetScissorCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
				x(x),
				y(y),
				width(width),
				height(height)
			{}

			void execute() const {
				glScissor(x, y, width, height);
			}

		private:
			const GLint x;
			const GLint y;
			const GLsizei width;
			const GLsizei height;
	};
}