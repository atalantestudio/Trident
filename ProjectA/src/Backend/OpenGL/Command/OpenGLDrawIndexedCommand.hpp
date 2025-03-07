#pragma once

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawIndexedCommand : public OpenGLCommand {
		public:
			OpenGLDrawIndexedCommand(const GLenum mode, const GLsizei count, const GLsizei type, const GLvoid* indices, const GLsizei instancecount);

			void execute() const override;

		private:
			const GLenum mode;
			const GLsizei count;
			const GLsizei type;
			const GLvoid* indices;
			const GLsizei instancecount;
	};
}