#pragma once

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawCommand : public OpenGLCommand {
		public:
			OpenGLDrawCommand(const GLenum mode, const GLint first, const GLsizei count, const GLsizei instancecount);

			void execute() const override;

		private:
			const GLenum mode;
			const GLint first;
			const GLsizei count;
			const GLsizei instancecount;
	};
}