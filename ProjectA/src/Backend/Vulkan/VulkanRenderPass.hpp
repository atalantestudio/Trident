#pragma once

#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			VulkanRenderPass(const VulkanSurface& surface, VkDevice device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

		private:
			const VkDevice device;
	};
}