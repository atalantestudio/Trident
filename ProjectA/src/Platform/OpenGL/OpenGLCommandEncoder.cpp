#include "pch.hpp"

#include <glad/glad.h>

#include "Platform/OpenGL/Command/OpenGLBeginRenderPassCommand.hpp"
#include "Platform/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"
#include "Platform/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"
#include "Platform/OpenGL/Command/OpenGLDrawCommand.hpp"
#include "Platform/OpenGL/OpenGLCommandBuffer.hpp"
#include "Platform/OpenGL/OpenGLCommandEncoder.hpp"

namespace ProjectA {
	OpenGLCommandEncoder::OpenGLCommandEncoder() {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLCommandEncoder]");
	}

	void OpenGLCommandEncoder::begin() const {}

	void OpenGLCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		commandBuffer.emplace_back(createRef<OpenGLBeginRenderPassCommand>(descriptor));
	}

	void OpenGLCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->renderPipeline = reinterpret_cast<const OpenGLRenderPipeline*>(renderPipeline.get());

		commandBuffer.emplace_back(createRef<OpenGLBindRenderPipelineCommand>(this->renderPipeline));
	}

	void OpenGLCommandEncoder::bindVertexBuffer(const ref<Buffer>& vertexBuffer, const uint32 stride) {
		commandBuffer.emplace_back(createRef<OpenGLBindVertexBufferCommand>(reinterpret_cast<const OpenGLBuffer*>(vertexBuffer.get()), stride));
	}

	void OpenGLCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) {}

	void OpenGLCommandEncoder::setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) {}

	void OpenGLCommandEncoder::draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) {
		commandBuffer.emplace_back(createRef<OpenGLDrawCommand>(renderPipeline->primitiveTopology, vertexCount, instanceCount, vertexOffset));
	}

	void OpenGLCommandEncoder::endRenderPass() const {}

	ref<CommandBuffer> OpenGLCommandEncoder::end() const {
		return createRef<OpenGLCommandBuffer>(commandBuffer);
	}

	void OpenGLCommandEncoder::reset() {
		renderPipeline = nullptr;
		commandBuffer.clear();
	}
}