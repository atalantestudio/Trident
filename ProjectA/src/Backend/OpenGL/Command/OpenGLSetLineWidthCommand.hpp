#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetLineWidthCommand : public OpenGLCommand {
		public:
			OpenGLSetLineWidthCommand(GLfloat width) :
				width(width)
			{}

			void execute() const override {
				//glLineWidth(width);
			}

		private:
			const GLfloat width;
	};
}