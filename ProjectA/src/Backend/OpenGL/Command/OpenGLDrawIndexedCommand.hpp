#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawIndexedCommand final : public OpenGLCommand {
		public:
			explicit OpenGLDrawIndexedCommand(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) :
				mode(mode),
				count(count),
				type(type),
				indices(indices),
				instancecount(instancecount),
				basevertex(basevertex),
				baseinstance(baseinstance)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_2
					glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instancecount, basevertex, baseinstance);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_2
					#pragma message "Using glDrawElementsInstancedBaseVertex. The baseinstance argument will be ignored."

					glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
					#pragma message "Using glDrawElementsInstanced. The basevertex and baseinstance arguments will be ignored."

					glDrawElementsInstanced(mode, count, type, indices, instancecount);
				#elif OPENGL_VERSION >= OPENGL_VERSION_2_0
					#pragma message "Using glDrawElements. The basevertex and baseinstance arguments will be ignored."

					for (GLint instanceindex = 0; instanceindex < instancecount; instanceindex += 1) {
						glDrawElements(mode, count, type, indices);
					}
				#else
					ENGINE_ERROR_UNSUPPORTED();
				#endif
			}

		private:
			const GLenum mode;
			const GLsizei count;
			const GLenum type;
			const GLvoid* indices;
			const GLsizei instancecount;
			const GLint basevertex;
			const GLuint baseinstance;
	};
}