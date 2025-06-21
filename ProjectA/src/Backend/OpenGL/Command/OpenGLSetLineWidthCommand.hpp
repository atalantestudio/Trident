#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetLineWidthCommand final : public OpenGLCommand {
		public:
			explicit OpenGLSetLineWidthCommand(GLfloat width) :
				width(width)
			{}

			void execute() const final override {
				glLineWidth(1.0f);
			}

		private:
			const GLfloat width;
	};
}