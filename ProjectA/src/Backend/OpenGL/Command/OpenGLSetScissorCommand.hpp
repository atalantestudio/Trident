#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"

namespace ProjectA {
	class OpenGLSetScissorCommand : public OpenGLCommand {
		public:
			OpenGLSetScissorCommand(GLint xOffset, GLint yOffset, Extent2D extent) :
				xOffset(xOffset),
				yOffset(yOffset),
				extent(extent)
			{}

			void execute() const final override {
				glScissor(xOffset, yOffset, extent.width, extent.height);
			}

		private:
			const GLint xOffset;
			const GLint yOffset;
			const Extent2D extent;
	};
}