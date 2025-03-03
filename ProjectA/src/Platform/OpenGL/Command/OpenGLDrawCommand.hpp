#pragma once

#include <glad/glad.h>

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawCommand : public OpenGLCommand {
		public:
			OpenGLDrawCommand(const GLenum primitiveTopology, const GLsizei vertexCount, const GLsizei instanceCount, const GLint vertexOffset);

			void execute() const override;

		private:
			const GLenum primitiveTopology;
			const GLsizei vertexCount;
			const GLsizei instanceCount;
			const GLint vertexOffset;
	};
}