#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"

#include "Engine/API/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLCommandEncoder : public CommandEncoder {
		public:
			OpenGLCommandEncoder(const OpenGLDevice& device, const CommandBufferDescriptor& descriptor);

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) override;

			void bindComputePipeline(const ref<ComputePipeline>& computePipeline) override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) override;

			void bindAllDescriptorSets() const override;

			void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) override;

			void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) override;

			void copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, uint32 width, uint32 height, uint32 depth) override;

			void dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) override;

			void setImageMemoryBarrier(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) override;

			void setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		protected:
			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) override;

		private:
			chain<ref<OpenGLCommand>> commandBuffer;
			ref<OpenGLComputePipeline> computePipeline = nullptr;
			ref<OpenGLRenderPipeline> renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;

			const PhysicalDeviceLimits& limits;
	};
}