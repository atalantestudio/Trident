#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLDrawCommand.hpp"

namespace ProjectA {
	OpenGLDrawCommand::OpenGLDrawCommand(const GLenum mode, const GLint first, const GLsizei count, const GLsizei instancecount) :
		mode(mode),
		first(first),
		count(count),
		instancecount(instancecount)
	{}

	void OpenGLDrawCommand::execute() const {
		glDrawArraysInstanced(mode, first, count, instancecount);
	}
}