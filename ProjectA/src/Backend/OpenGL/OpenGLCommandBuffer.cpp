#include "pch.hpp"

#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"

namespace ProjectA {
	OpenGLCommandBuffer::OpenGLCommandBuffer(const std::vector<ref<OpenGLCommand>>& commandBuffer) :
		CommandBuffer(static_cast<uint32>(commandBuffer.size())),
		ObjectWrapper(commandBuffer)
	{}

	std::vector<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::begin() const {
		return object.begin();
	}

	std::vector<ref<OpenGLCommand>>::const_iterator OpenGLCommandBuffer::end() const {
		return object.end();
	}
}