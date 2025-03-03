#include "pch.hpp"

#include "Platform/OpenGL/OpenGLCommandBuffer.hpp"

namespace ProjectA {
	OpenGLCommandBuffer::OpenGLCommandBuffer(const std::vector<ref<OpenGLCommand>>& commandBuffer) :
		ObjectWrapper(commandBuffer)
	{}

	std::vector<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::begin() const {
		return object.begin();
	}

	std::vector<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::end() const {
		return object.end();
	}
}