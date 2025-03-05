#pragma once

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindVertexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindVertexBufferCommand(const GLuint bindingindex, const GLuint buffer, const GLsizei stride);

			void execute() const override;

		private:
			const GLuint bindingindex;
			const GLuint buffer;
			const GLsizei stride;
	};
}