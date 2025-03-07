#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLSetViewportCommand.hpp"

namespace ProjectA {
	OpenGLSetViewportCommand::OpenGLSetViewportCommand(const GLint x, const GLint y, const GLsizei width, const GLsizei height) :
		x(x),
		y(y),
		width(width),
		height(height)
	{}

	void OpenGLSetViewportCommand::execute() const {
		glViewport(x, y, width, height);
	}
}