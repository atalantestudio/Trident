#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLDrawIndexedIndirectCommand final : public OpenGLCommand {
		public:
			explicit OpenGLDrawIndexedIndirectCommand(GLenum mode, GLenum type, GLenum buffer, const GLubyte* indirect, GLsizei drawcount, GLsizei stride) :
				mode(mode),
				type(type),
				buffer(buffer),
				indirect(indirect),
				drawcount(drawcount),
				stride(stride)
			{}

			void execute() const final override {
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);

				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glMultiDrawElementsIndirect(mode, type, indirect, drawcount, stride);
				#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
					for (GLsizei drawindex = 0; drawindex < drawcount; drawindex += 1) {
						glDrawElementsIndirect(mode, type, indirect + drawindex * stride);
					}
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLenum mode;
			const GLenum type;
			const GLenum buffer;
			const GLubyte* indirect;
			const GLsizei drawcount;
			const GLsizei stride;
	};
}