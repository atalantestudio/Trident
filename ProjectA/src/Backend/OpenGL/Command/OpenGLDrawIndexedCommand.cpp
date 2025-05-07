#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLDrawIndexedCommand.hpp"

namespace ProjectA {
	OpenGLDrawIndexedCommand::OpenGLDrawIndexedCommand(GLenum mode, GLsizei count, GLsizei type, const GLvoid* indices, GLsizei instancecount) :
		mode(mode),
		count(count),
		type(type),
		indices(indices),
		instancecount(instancecount)
	{}

	void OpenGLDrawIndexedCommand::execute() const {
		if (instancecount == 1) {
			glDrawElements(mode, count, type, indices);
		}
		else {
			glDrawElementsInstanced(mode, count, type, indices, instancecount);
		}
	}
}