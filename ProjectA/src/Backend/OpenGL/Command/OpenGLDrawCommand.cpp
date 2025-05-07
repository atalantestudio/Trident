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
		if (instancecount == 1) {
			glDrawArrays(mode, first, count);
		}
		else {
			glDrawArraysInstanced(mode, first, count, instancecount);
		}
	}
}