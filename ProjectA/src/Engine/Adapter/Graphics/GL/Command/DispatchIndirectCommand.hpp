#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLDispatchIndirectCommand final : public OpenGLCommand {
		public:
			explicit OpenGLDispatchIndirectCommand(GLuint buffer, GLintptr indirect) :
				buffer(buffer),
				indirect(indirect)
			{}

			void execute() const final override {
				glBindBuffer(GL_DISPATCH_INDIRECT_BUFFER, buffer);

				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glDispatchComputeIndirect(indirect);
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLuint buffer;
			const GLintptr indirect;
	};
}