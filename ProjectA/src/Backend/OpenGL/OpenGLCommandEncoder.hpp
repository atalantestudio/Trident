#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

#include "Engine/API/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLCommandEncoder : public CommandEncoder {
		public:
			OpenGLCommandEncoder(const CommandBufferDescriptor& descriptor);

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) override;

			void bindDescriptorSets(uint32 descriptorSetIndex) const override;

			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, IndexType indexType) override;

			void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint32 stride) override;

			void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) override;

			void copyBufferToTexture(const ref<Buffer>& sourceBuffer, const ref<Texture>& destinationTexture, uint32 width, uint32 height, uint32 depth) override;

			void setTextureMemoryBarrier(const ref<Texture>& texture, TextureLayout oldLayout, TextureLayout newLayout) const override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) override;

			void setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		private:
			chain<ref<OpenGLCommand>> commandBuffer;
			const OpenGLRenderPipeline* renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;
	};
}