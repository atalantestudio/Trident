// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class GL::DrawCommand final : public GL::Command {
		public:
			explicit DrawCommand(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance) :
				mode(mode),
				first(first),
				count(count),
				instancecount(instancecount),
				baseinstance(baseinstance)
			{}

			void execute() const final override {
				#if OPENGL_VERSION >= OPENGL_VERSION_4_2
					glDrawArraysInstancedBaseInstance(mode, first, count, instancecount, baseinstance);
				#elif OPENGL_VERSION >= OPENGL_VERSION_3_1
					#pragma message("Using glDrawArraysInstanced. The baseinstance argument will be ignored.")

					glDrawArraysInstanced(mode, first, count, instancecount);
				#else
					#pragma message("Using glDrawArrays. The baseinstance argument will be ignored.")

					for (GLint instanceindex = 0; instanceindex < instancecount; instanceindex += 1) {
						glDrawArrays(mode, first, count);
					}
				#endif
			}

		private:
			GLenum mode;
			GLint first;
			GLsizei count;
			GLsizei instancecount;
			GLuint baseinstance;
	};
}