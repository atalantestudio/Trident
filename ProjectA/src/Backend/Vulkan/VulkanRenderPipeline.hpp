#pragma once

#include <vulkan/vulkan.h>

#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanRenderPipeline(const VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

		public:
			const VulkanPipelineLayout* pipelineLayout;

		private:
			const VkDevice device;
	};
}