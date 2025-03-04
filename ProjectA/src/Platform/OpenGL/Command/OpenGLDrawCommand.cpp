#include "pch.hpp"

#include "Platform/OpenGL/Command/OpenGLDrawCommand.hpp"

namespace ProjectA {
	OpenGLDrawCommand::OpenGLDrawCommand(const GLenum mode, const GLint first, const GLsizei count, const GLsizei instancecount) :
		mode(mode),
		first(first),
		count(count),
		instancecount(instancecount)
	{}

	void OpenGLDrawCommand::execute() const {
		if (instancecount > 1) {
			glDrawArraysInstanced(mode, first, count, instancecount);
		}
		else {
			glDrawArrays(mode, first, count);
		}
	}
}