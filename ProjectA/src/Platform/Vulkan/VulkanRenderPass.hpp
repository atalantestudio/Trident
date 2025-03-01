#pragma once

#include <vulkan/vulkan.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/RenderPass.hpp"

#include "Platform/Vulkan/VulkanSurface.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			VulkanRenderPass(const VulkanSurface& surface, const VkDevice device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

		private:
			const VkDevice device;
	};
}