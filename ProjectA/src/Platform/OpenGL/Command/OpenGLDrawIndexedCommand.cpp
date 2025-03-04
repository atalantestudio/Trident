#include "pch.hpp"

#include "Platform/OpenGL/Command/OpenGLDrawIndexedCommand.hpp"

namespace ProjectA {
	OpenGLDrawIndexedCommand::OpenGLDrawIndexedCommand(const GLenum mode, const GLsizei count, const GLsizei type, const GLvoid* indices, const GLsizei instancecount) :
		mode(mode),
		count(count),
		type(type),
		indices(indices),
		instancecount(instancecount)
	{}

	void OpenGLDrawIndexedCommand::execute() const {
		if (instancecount > 1) {
			glDrawElementsInstanced(mode, count, type, indices, instancecount);
		}
		else {
			glDrawElements(mode, count, type, indices);
		}
	}
}