#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLDrawCommand.hpp"

namespace ProjectA {
	OpenGLDrawCommand::OpenGLDrawCommand(GLenum mode, GLint first, GLsizei count, GLsizei instancecount) :
		mode(mode),
		first(first),
		count(count),
		instancecount(instancecount)
	{}

	void OpenGLDrawCommand::execute() const {
		glDrawArraysInstanced(mode, first, count, instancecount);
	}
}