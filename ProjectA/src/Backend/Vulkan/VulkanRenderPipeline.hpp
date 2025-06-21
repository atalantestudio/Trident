#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"
#include "Backend/Vulkan/VulkanPipelineLayout.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Engine/API/Rendering/Descriptor/RenderPipelineDescriptor.hpp"
#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline final : public RenderPipeline, public VulkanPipeline {
		public:
			explicit VulkanRenderPipeline(VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline() final override;

		private:
			const VkDevice device;
	};
}