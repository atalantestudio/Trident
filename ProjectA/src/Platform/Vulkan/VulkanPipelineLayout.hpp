#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/PipelineLayout.hpp"

namespace ProjectA {
	class VulkanPipelineLayout : public PipelineLayout, public ObjectWrapper<VkPipelineLayout> {
		public:
			VulkanPipelineLayout(const VkDevice device, const PipelineLayoutDescriptor& descriptor);

			~VulkanPipelineLayout();

		private:
			const VkDevice device;
	};
}