#pragma once

#include "Backend/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLDrawIndirectCommand final : public OpenGLCommand {
		public:
			explicit OpenGLDrawIndirectCommand(GLenum mode, GLenum buffer, const GLubyte* indirect, GLsizei drawcount, GLsizei stride) :
				mode(mode),
				buffer(buffer),
				indirect(indirect),
				drawcount(drawcount),
				stride(stride)
			{}

			void execute() const final override {
				glBindBuffer(GL_DRAW_INDIRECT_BUFFER, buffer);

				#if OPENGL_VERSION >= OPENGL_VERSION_4_3
					glMultiDrawArraysIndirect(mode, indirect, drawcount, stride);
				#elif OPENGL_VERSION >= OPENGL_VERSION_4_0
					for (GLsizei drawindex = 0; drawindex < drawcount; drawindex += 1) {
						glDrawArraysIndirect(mode, indirect + drawindex * stride);
					}
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLenum mode;
			const GLenum buffer;
			const GLubyte* indirect;
			const GLsizei drawcount;
			const GLsizei stride;
	};
}