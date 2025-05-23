#pragma once

#include "Backend/OpenGL/Command/OpenGLCommand.hpp"
#include "Backend/OpenGL/OpenGLComputePipeline.hpp"
#include "Backend/OpenGL/OpenGLDevice.hpp"
#include "Backend/OpenGL/OpenGLRenderPass.hpp"
#include "Backend/OpenGL/OpenGLRenderPipeline.hpp"
#include "Backend/OpenGL/OpenGLSwapchain.hpp"

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class OpenGLCommandEncoder final : public CommandEncoder {
		public:
			OpenGLCommandEncoder(const OpenGLDevice& device, const CommandBufferDescriptor& descriptor);

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(const ref<ComputePipeline>& computePipeline) final override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) final override;

			void bindAllDescriptorSets() final override;

			void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) final override;

			void copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, uint32 width, uint32 height, uint32 depth) final override;

			void dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void setImageMemoryBarrier(const ref<Image>& image, ImageLayout oldLayout, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, uint32 width, uint32 height) final override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) final override;

			void endRenderPass() final override;

			ref<CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

		private:
			OpenGLRenderPass* renderPass = nullptr;
			OpenGLSwapchain* swapchain = nullptr;

			std::vector<ref<OpenGLCommand>> commandBuffer;
			ref<OpenGLComputePipeline> computePipeline = nullptr;
			ref<OpenGLRenderPipeline> renderPipeline = nullptr;
			GLuint indexOffset = 0;
			GLenum indexType = 0;
			GLsizei indexTypeSize = 0;

			const PhysicalDeviceLimits& limits;
	};
}