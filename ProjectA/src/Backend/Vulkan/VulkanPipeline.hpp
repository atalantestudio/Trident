#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanPipelineLayout.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Pipeline.hpp"

namespace ProjectA {
	class VulkanPipeline : public Pipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanPipeline(VkPipelineBindPoint bindPoint, const VulkanPipelineLayout* layout);

		public:
			VkPipelineBindPoint bindPoint;
			const VulkanPipelineLayout* layout;
	};
}