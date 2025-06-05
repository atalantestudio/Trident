#pragma once

#include "Backend/Vulkan/VulkanPipelineLayout.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Pipeline.hpp"

namespace ProjectA {
	class VulkanPipeline : public Pipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanPipeline(VkPipelineBindPoint bindPoint, const VulkanPipelineLayout* layout);

		public:
			VkPipelineBindPoint bindPoint;
			const VulkanPipelineLayout* layout;
	};
}