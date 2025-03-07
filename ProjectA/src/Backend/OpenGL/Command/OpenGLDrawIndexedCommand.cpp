#include "pch.hpp"

#include "Backend/OpenGL/Command/OpenGLDrawIndexedCommand.hpp"

namespace ProjectA {
	OpenGLDrawIndexedCommand::OpenGLDrawIndexedCommand(const GLenum mode, const GLsizei count, const GLsizei type, const GLvoid* indices, const GLsizei instancecount) :
		mode(mode),
		count(count),
		type(type),
		indices(indices),
		instancecount(instancecount)
	{}

	void OpenGLDrawIndexedCommand::execute() const {
		glDrawElementsInstanced(mode, count, type, indices, instancecount);
	}
}