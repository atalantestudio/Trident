#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPipeline.hpp"

#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public ObjectWrapper<VkPipeline> {
		public:
			VulkanRenderPipeline(const VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

		private:
			const VkDevice device;
	};
}