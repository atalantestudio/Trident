#pragma once

#include "Backend/Vulkan/VulkanPipeline.hpp"

#include "Engine/API/Rendering/RenderPipeline.hpp"

namespace ProjectA {
	class VulkanRenderPipeline final : public VulkanPipeline, public RenderPipeline {
		public:
			explicit VulkanRenderPipeline(VkDevice device, const RenderPipelineDescriptor& descriptor);

			~VulkanRenderPipeline() final override;

		private:
			const VkDevice device;
	};
}