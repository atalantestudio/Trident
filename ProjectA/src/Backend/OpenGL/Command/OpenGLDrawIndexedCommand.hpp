#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLDrawIndexedCommand : public OpenGLCommand {
		public:
			OpenGLDrawIndexedCommand(GLenum mode, GLsizei count, GLsizei type, const GLvoid* indices, GLsizei instancecount);

			void execute() const override;

		private:
			const GLenum mode;
			const GLsizei count;
			const GLsizei type;
			const GLvoid* indices;
			const GLsizei instancecount;
	};
}