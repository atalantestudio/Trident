#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Pipeline.hpp"

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class VulkanCommandEncoder final : public CommandEncoder {
		public:
			explicit VulkanCommandEncoder(const VulkanDevice& device, VkCommandPool commandPool, const CommandBufferDescriptor& descriptor);

			~VulkanCommandEncoder() final override;

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(ref<ComputePipeline> computePipeline) final override;

			void bindRenderPipeline(ref<RenderPipeline> renderPipeline) final override;

			void bindDescriptorSets(ref<PipelineLayout> pipelineLayout, const sequence<ref<DescriptorSet>>& descriptorSets) final override;

			void bindVertexBuffer(uint32 bindingIndex, ref<Buffer> vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(ref<Buffer> sourceBuffer, ref<Buffer> destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) final override;

			void copyBufferToImage(ref<Buffer> sourceBuffer, ref<Image> destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void copyImageToBuffer(ref<Image> sourceImage, ref<Buffer> destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void dispatch(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void dispatchIndirect(ref<Buffer> buffer, uint64 bufferOffset) final override;

			void nextSubpass() final override;

			void setImageMemoryBarrier(ref<Image> image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) final override;

			void setViewport(vec2<float32> origin, vec2<float32> size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset, uint32 instanceOffset) final override;

			void drawIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset, sint32 indexBias, uint32 instanceOffset) final override;

			void drawIndexedIndirect(ref<Buffer> buffer, uint64 bufferOffset, uint32 drawCount, uint32 stride) final override;

			void endRenderPass() final override;

			ref<CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(ref<Buffer> indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

			static sequence<VkBufferImageCopy> parseBufferImageCopyRegions(const sequence<BufferImageCopyRegion>& copyRegions);

		private:
			const VkDevice device;
			const VkCommandPool commandPool;
			const VkCommandBufferUsageFlags commandBufferUsageFlag;
			VkCommandBuffer commandBuffer;
			uint32 commandCount = 0;
			ref<VulkanPipeline> pipeline;
			PhysicalDeviceLimits limits;
	};
}