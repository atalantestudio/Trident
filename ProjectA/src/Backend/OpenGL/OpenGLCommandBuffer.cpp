#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"

namespace ProjectA {
	OpenGLCommandBuffer::OpenGLCommandBuffer(const chain<ref<OpenGLCommand>>& commandBuffer) :
		ObjectWrapper(commandBuffer)
	{}

	chain<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::begin() const {
		return object.begin();
	}

	chain<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::end() const {
		return object.end();
	}
}