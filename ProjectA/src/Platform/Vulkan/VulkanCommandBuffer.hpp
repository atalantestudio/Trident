#pragma once

#include <vulkan/vulkan.h>

#include "Engine/Renderer/CommandBuffer.hpp"

namespace ProjectA {
	class VulkanCommandBuffer : public CommandBuffer {
		public:
			VulkanCommandBuffer(const VkDevice device, const VkCommandPool commandPool);

			~VulkanCommandBuffer();

			void begin() const override;

			void beginRenderPass(const BeginRenderPassDescriptor& descriptor) const override;

			void bindRenderPipeline(const ref<RenderPipeline>& renderPipeline) const override;

			void bindVertexBuffer(const ref<Buffer>& vertexBuffer) const override;

			void setViewport(const vec2<float32>& origin, const vec2<float32>& size, const float32 minDepth, const float32 maxDepth) const override;

			void setScissor(const int32 xOffset, const int32 yOffset, const uint32 width, const uint32 height) const override;

			void draw(const uint32 vertexCount, const uint32 instanceCount, const uint32 vertexOffset) const override;

			void endRenderPass() const override;

			void end() const override;

			void reset() const override;

		public:
			VkCommandBuffer object;

		private:
			const VkDevice device;
			const VkCommandPool commandPool;
	};
}