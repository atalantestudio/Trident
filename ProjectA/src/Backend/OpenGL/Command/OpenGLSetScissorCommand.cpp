#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLSetScissorCommand.hpp"

namespace ProjectA {
	OpenGLSetScissorCommand::OpenGLSetScissorCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
		x(x),
		y(y),
		width(width),
		height(height)
	{}

	void OpenGLSetScissorCommand::execute() const {
		glScissor(x, y, width, height);
	}
}