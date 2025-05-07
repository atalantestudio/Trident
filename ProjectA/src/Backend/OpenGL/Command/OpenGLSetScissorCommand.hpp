#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetScissorCommand : public OpenGLCommand {
		public:
			OpenGLSetScissorCommand(GLint x, GLint y, GLsizei width, GLsizei height);

			void execute() const;

		private:
			GLint x;
			GLint y;
			GLsizei width;
			GLsizei height;
	};
}