// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace Trident {
	class GL::DrawIndexedCommand final : public GL::Command {
		public:
			explicit DrawIndexedCommand(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance) :
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
					#pragma message("Using glDrawElementsInstancedBaseVertex. The baseinstance argument will be ignored.")

					glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
					#pragma message("Using glDrawElementsInstanced. The basevertex and baseinstance arguments will be ignored.")

					glDrawElementsInstanced(mode, count, type, indices, instancecount);
				#else
					#pragma message("Using glDrawElements. The basevertex and baseinstance arguments will be ignored.")

					for (GLint instanceindex = 0; instanceindex < instancecount; instanceindex += 1) {
						glDrawElements(mode, count, type, indices);
					}
				#endif
			}

		private:
			GLenum mode;
			GLsizei count;
			GLenum type;
			const GLvoid* indices;
			GLsizei instancecount;
			GLint basevertex;
			GLuint baseinstance;
	};
}