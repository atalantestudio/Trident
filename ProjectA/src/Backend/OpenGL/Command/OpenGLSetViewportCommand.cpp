#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLSetViewportCommand.hpp"

namespace ProjectA {
	OpenGLSetViewportCommand::OpenGLSetViewportCommand(GLint x, GLint y, GLsizei width, GLsizei height) :
		x(x),
		y(y),
		width(width),
		height(height)
	{}

	void OpenGLSetViewportCommand::execute() const {
		glViewport(x, y, width, height);
	}
}