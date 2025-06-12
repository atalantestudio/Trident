#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawIndexedCommand : public OpenGLCommand {
		public:
			OpenGLDrawIndexedCommand(GLenum mode, GLsizei count, GLsizei type, const GLvoid* indices, GLsizei instancecount) :
				mode(mode),
				count(count),
				type(type),
				indices(indices),
				instancecount(instancecount)
			{}

			void execute() const override {
				glDrawElementsInstanced(mode, count, type, indices, instancecount);
			}

		private:
			const GLenum mode;
			const GLsizei count;
			const GLsizei type;
			const GLvoid* indices;
			const GLsizei instancecount;
	};
}