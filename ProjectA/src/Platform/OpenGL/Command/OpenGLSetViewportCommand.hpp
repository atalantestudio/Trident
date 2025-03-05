#pragma once

#include <glad/glad.h>

#include "Platform/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetViewportCommand : public OpenGLCommand {
		public:
			OpenGLSetViewportCommand(const GLint x, const GLint y, const GLsizei width, const GLsizei height);

			void execute() const override;

		private:
			const GLint x;
			const GLint y;
			const GLsizei width;
			const GLsizei height;
	};
}