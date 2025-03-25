#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLBuffer.hpp"

namespace ProjectA {
	class OpenGLBindIndexBufferCommand : public OpenGLCommand {
		public:
			OpenGLBindIndexBufferCommand(GLuint indexBuffer);

			void execute() const override;

		private:
			const GLuint indexBuffer;
	};
}