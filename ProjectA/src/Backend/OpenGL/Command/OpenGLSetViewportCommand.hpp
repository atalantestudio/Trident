#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetViewportCommand : public OpenGLCommand {
		public:
			OpenGLSetViewportCommand(GLint x, GLint y, GLsizei width, GLsizei height);

			void execute() const override;

		private:
			const GLint x;
			const GLint y;
			const GLsizei width;
			const GLsizei height;
	};
}