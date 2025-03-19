#pragma once

#include "Engine/Renderer/CommandEncoder.hpp"

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

namespace ProjectA {
	class OpenGLCommandEncoder : public CommandEncoder {
		public:
			OpenGLCommandEncoder(const CommandBufferDescriptor& descriptor);

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) override;

			void bindDescriptorSets(const uint32 descriptorSetIndex) const override;

			void bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) override;

			void bindVertexBuffer(const uint32 bindingIndex, const ref<Buffer>& vertexBuffer, const uint32 stride) override;

			void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) override;

			void copyBufferToTexture(const ref<Buffer>& sourceBuffer, const ref<Texture>& destinationTexture, uint32 width, uint32 height, uint32 depth) override;

			void setTextureMemoryBarrier(const ref<Texture>& texture, TextureLayout oldLayout, TextureLayout newLayout) const override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) override;

			void setScissor(const sint32 xOffset, const sint32 yOffset, const uint32 width, const uint32 height) override;

			void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) override;

			void drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		private:
			std::vector<ref<OpenGLCommand>> commandBuffer;
			const OpenGLRenderPipeline* renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;
	};
}