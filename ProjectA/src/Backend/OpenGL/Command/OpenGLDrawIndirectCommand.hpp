#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

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
				#endif
			}

		private:
			GLenum mode;
			GLenum buffer;
			const GLubyte* indirect;
			GLsizei drawcount;
			GLsizei stride;
	};
}