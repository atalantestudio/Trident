#pragma once

#include "Engine/Adapter/Graphics/GL/Command/Command.hpp"

namespace ProjectA {
	class OpenGLSetLineWidthCommand final : public OpenGLCommand {
		public:
			explicit OpenGLSetLineWidthCommand(GLfloat width) :
				width(width)
			{}

			void execute() const final override {
				glLineWidth(width);
			}

		private:
			const GLfloat width;
	};
}