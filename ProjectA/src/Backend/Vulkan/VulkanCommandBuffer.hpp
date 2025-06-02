#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/CommandBuffer.hpp"

namespace ProjectA {
	class VulkanCommandBuffer : public CommandBuffer, public ObjectWrapper<const VkCommandBuffer> {
		public:
			VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount);

		public:
			const VkPipelineStageFlags pipelineStageFlags;
	};
}