#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanPipeline.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public VulkanPipeline {
		public:
			VulkanRenderPipeline(VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

		private:
			const VkDevice device;
	};
}