#pragma once

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindVertexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindVertexBufferCommand(const OpenGLBuffer* vertexBuffer, const GLsizei stride);

			void execute() const override;

		private:
			const OpenGLBuffer* vertexBuffer;
			const GLsizei stride;
	};
}