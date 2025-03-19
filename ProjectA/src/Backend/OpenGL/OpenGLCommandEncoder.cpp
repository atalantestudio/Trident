#include "pch.hpp"

#include <glad/glad.h>

#include "Backend/OpenGL/Command/OpenGLBeginRenderPassCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindIndexBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindRenderPipelineCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLBindVertexBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLCopyBufferToBufferCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLCopyBufferToTextureCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLDrawCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLDrawIndexedCommand.hpp"
#include "Backend/OpenGL/Command/OpenGLSetViewportCommand.hpp"
#include "Backend/OpenGL/IndexType.hpp"
#include "Backend/OpenGL/OpenGLCommandBuffer.hpp"
#include "Backend/OpenGL/OpenGLCommandEncoder.hpp"
#include "Backend/OpenGL/OpenGLTexture.hpp"

namespace ProjectA {
	OpenGLCommandEncoder::OpenGLCommandEncoder(const CommandBufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCSIG);

		ENGINE_TRACE("[OpenGLCommandEncoder]");
	}

	void OpenGLCommandEncoder::begin() const {}

	void OpenGLCommandEncoder::beginRenderPass(const BeginRenderPassDescriptor& descriptor) {
		commandBuffer.emplace_back(createRef<OpenGLBeginRenderPassCommand>(descriptor));
	}

	void OpenGLCommandEncoder::bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) {
		this->renderPipeline = static_cast<const OpenGLRenderPipeline*>(renderPipeline.get());

		commandBuffer.emplace_back(createRef<OpenGLBindRenderPipelineCommand>(this->renderPipeline));
	}

	void OpenGLCommandEncoder::bindDescriptorSets(const uint32 descriptorSetIndex) const {}

	void OpenGLCommandEncoder::bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) {
		indexOffset = static_cast<uint32>(offset);
		this->indexType = getGlIndexType(indexType);
		indexTypeSize = getGlIndexTypeSize(indexType);

		commandBuffer.emplace_back(createRef<OpenGLBindIndexBufferCommand>(static_cast<const OpenGLBuffer*>(indexBuffer.get())->object));
	}

	void OpenGLCommandEncoder::bindVertexBuffer(const uint32 bindingIndex, const ref<Buffer>& vertexBuffer, const uint32 stride) {
		commandBuffer.emplace_back(createRef<OpenGLBindVertexBufferCommand>(
			bindingIndex,
			static_cast<const OpenGLBuffer*>(vertexBuffer.get())->object,
			stride
		));
	}

	void OpenGLCommandEncoder::copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) {
		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToBufferCommand>(
			static_cast<const OpenGLBuffer*>(sourceBuffer.get())->object,
			static_cast<const OpenGLBuffer*>(destinationBuffer.get())->object,
			sourceOffset,
			destinationOffset,
			size
		));
	}

	void OpenGLCommandEncoder::copyBufferToTexture(const ref<Buffer>& sourceBuffer, const ref<Texture>& destinationTexture, uint32 width, uint32 height, uint32 depth) {
		commandBuffer.emplace_back(createRef<OpenGLCopyBufferToTextureCommand>(
			static_cast<const OpenGLBuffer*>(sourceBuffer.get())->object,
			static_cast<const OpenGLTexture*>(destinationTexture.get())->object,
			width,
			height,
			depth
		));
	}

	void OpenGLCommandEncoder::setTextureMemoryBarrier(const ref<Texture>& texture, TextureLayout oldLayout, TextureLayout newLayout) const {}

	void OpenGLCommandEncoder::setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) {
		commandBuffer.emplace_back(createRef<OpenGLSetViewportCommand>(
			// TODO: Use provided origin.
			// TODO: Use vec2<sint32>.
			static_cast<GLint>(0),
			static_cast<GLint>(0),
			// TODO: Use vec2<sint32>.
			static_cast<GLsizei>(lyah::abs(size[0])),
			static_cast<GLsizei>(lyah::abs(size[1]))
		));
	}

	void OpenGLCommandEncoder::setScissor(const sint32 xOffset, const sint32 yOffset, const uint32 width, const uint32 height) {}

	void OpenGLCommandEncoder::draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) {
		commandBuffer.emplace_back(createRef<OpenGLDrawCommand>(renderPipeline->primitiveTopology, vertexOffset, vertexCount, instanceCount));
	}

	void OpenGLCommandEncoder::drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) {
		commandBuffer.emplace_back(createRef<OpenGLDrawIndexedCommand>(renderPipeline->primitiveTopology, indexCount, indexType, reinterpret_cast<const GLvoid*>(static_cast<std::uintptr_t>(this->indexOffset + indexOffset * indexTypeSize)), instanceCount));
	}

	void OpenGLCommandEncoder::endRenderPass() const {}

	ref<CommandBuffer> OpenGLCommandEncoder::end() const {
		return createRef<OpenGLCommandBuffer>(commandBuffer);
	}

	void OpenGLCommandEncoder::reset() {
		commandBuffer.clear();
		renderPipeline = nullptr;
		indexOffset = 0;
		indexType = 0;
		indexTypeSize = 0;
	}
}