#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawCommand : public OpenGLCommand {
		public:
			OpenGLDrawCommand(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);

			void execute() const override;

		private:
			const GLenum mode;
			const GLint first;
			const GLsizei count;
			const GLsizei instancecount;
	};
}