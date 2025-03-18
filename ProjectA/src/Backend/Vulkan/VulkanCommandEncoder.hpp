#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/CommandEncoder.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"

namespace ProjectA {
	class VulkanCommandEncoder : public CommandEncoder {
		public:
			VulkanCommandEncoder(const VulkanDevice& device, const VkCommandPool commandPool, const CommandBufferDescriptor& descriptor);

			~VulkanCommandEncoder();

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) override;

			void bindDescriptorSets(const uint32 descriptorSetIndex) const override;

			void bindIndexBuffer(const ref<Buffer>& indexBuffer, const uint64 offset, const IndexType indexType) override;

			void bindVertexBuffer(const uint32 bindingIndex, const ref<Buffer>& vertexBuffer, const uint32 stride) override;

			void copyBuffer(const ref<Buffer>& sourceBuffer, const ref<Buffer>& destinationBuffer, uint64 sourceOffset, uint64 destinationOffset, uint64 size) override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) override;

			void setScissor(const sint32 xOffset, const sint32 yOffset, const uint32 width, const uint32 height) override;

			void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) override;

			void drawIndexed(const uint32 indexCount, const uint32 instanceCount, const uint32 indexOffset) override;

			void endRenderPass() const override;

			ref<CommandBuffer> end() const override;

			void reset() override;

		private:
			const VulkanDevice& device;
			const VkCommandPool commandPool;
			const VkCommandBufferUsageFlags commandBufferUsageFlag;
			VkCommandBuffer commandBuffer;
			const VulkanPipelineLayout* pipelineLayout = nullptr;
	};
}