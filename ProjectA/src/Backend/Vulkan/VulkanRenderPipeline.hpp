#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/Renderer/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline : public RenderPipeline, public VulkanPipeline {
		public:
			VulkanRenderPipeline(VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline();

		public:
			#ifdef BUILD_MODE_DEBUG
				const DynamicState dynamicState;
			#endif

		private:
			const VkDevice device;
	};
}