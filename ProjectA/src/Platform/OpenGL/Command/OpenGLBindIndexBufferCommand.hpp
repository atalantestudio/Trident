#pragma once

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindIndexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindIndexBufferCommand(const GLuint indexBuffer);

			void execute() const override;

		private:
			const GLuint indexBuffer;
	};
}