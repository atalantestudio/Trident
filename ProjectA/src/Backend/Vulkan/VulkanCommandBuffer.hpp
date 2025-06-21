#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/CommandBuffer.hpp"

namespace ProjectA {
	class VulkanCommandBuffer final : public CommandBuffer, public ObjectWrapper<const VkCommandBuffer> {
		public:
			explicit VulkanCommandBuffer(VkCommandBuffer commandBuffer, VkPipelineStageFlags pipelineStageFlags, uint32 commandCount);

		public:
			const VkPipelineStageFlags pipelineStageFlags;
	};
}