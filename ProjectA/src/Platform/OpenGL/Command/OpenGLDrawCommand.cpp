#include "pch.hpp"

#include "Platform/OpenGL/Command/OpenGLDrawCommand.hpp"

namespace ProjectA {
	OpenGLDrawCommand::OpenGLDrawCommand(const GLenum primitiveTopology, const GLsizei vertexCount, const GLsizei instanceCount, const GLint vertexOffset) :
		primitiveTopology(primitiveTopology),
		vertexCount(vertexCount),
		instanceCount(instanceCount),
		vertexOffset(vertexOffset)
	{}

	void OpenGLDrawCommand::execute() const {
		if (instanceCount > 1) {
			glDrawArraysInstanced(primitiveTopology, vertexOffset, vertexCount, instanceCount);
		}
		else {
			glDrawArrays(primitiveTopology, vertexOffset, vertexCount);
		}
	}
}