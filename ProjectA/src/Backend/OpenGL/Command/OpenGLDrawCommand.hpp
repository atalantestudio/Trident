#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawCommand : public OpenGLCommand {
		public:
			OpenGLDrawCommand(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) :
				mode(mode),
				first(first),
				count(count),
				instancecount(instancecount)
			{}

			void execute() const override {
				if (instancecount == 1) {
					glDrawArrays(mode, first, count);
				}
				else {
					glDrawArraysInstanced(mode, first, count, instancecount);
				}
			}

		private:
			const GLenum mode;
			const GLint first;
			const GLsizei count;
			const GLsizei instancecount;
	};
}