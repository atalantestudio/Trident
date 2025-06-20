#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/API/Rendering/CommandEncoder.hpp"

namespace ProjectA {
	class VulkanCommandEncoder final : public CommandEncoder {
		public:
			VulkanCommandEncoder(const VulkanDevice& device, VkCommandPool commandPool, const CommandBufferDescriptor& descriptor);

			~VulkanCommandEncoder() final override;

			void begin() const final override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) final override;

			void bindComputePipeline(const ref<ComputePipeline>& computePipeline) final override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) final override;

			void bindAllDescriptorSets() final override;

			void bindVertexBuffer(uint32 bindingIndex, const ref<Buffer>& vertexBuffer, uint64 offset, uint32 stride) final override;

			void copyBufferToBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, const sequence<BufferCopyRegion>& copyRegions) final override;

			void copyBufferToImage(const ref<Buffer>& sourceBuffer, const ref<Image>& destinationImage, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void copyImageToBuffer(const ref<Image>& sourceImage, const ref<Buffer>& destinationBuffer, const sequence<BufferImageCopyRegion>& copyRegions) final override;

			void dispatchGroups(uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) final override;

			void setImageMemoryBarrier(const ref<Image>& image, ImageLayout newLayout, PipelineStage sourceStage, PipelineStage destinationStage) final override;

			void setLineWidth(float32 lineWidth) final override;

			void setScissor(sint32 xOffset, sint32 yOffset, Extent2D extent) final override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, float32 minDepth, float32 maxDepth) final override;

			void draw(uint32 vertexCount, uint32 instanceCount, uint32 vertexOffset) final override;

			void drawIndexed(uint32 indexCount, uint32 instanceCount, uint32 indexOffset) final override;

			void endRenderPass() final override;

			ref<CommandBuffer> end() const final override;

			void reset() final override;

		protected:
			void bindIndexBuffer(const ref<Buffer>& indexBuffer, uint64 offset, uint8 indexTypeSize) final override;

			static sequence<VkBufferImageCopy> parseBufferImageCopyRegions(const sequence<BufferImageCopyRegion>& copyRegions);

		private:
			const VkDevice device;
			const VkCommandPool commandPool;
			const VkCommandBufferUsageFlags commandBufferUsageFlag;
			VkCommandBuffer commandBuffer;
			uint32 commandCount = 0;
			ref<VulkanPipeline> pipeline;

			const PhysicalDeviceLimits& limits;
	};
}