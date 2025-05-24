#pragma once

#include "Backend/Vulkan/VulkanImageView.hpp"
#include "Backend/Vulkan/VulkanSurface.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		public:
			VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

			ref<Image> createColorAttachmentImage(const AttachmentImageDescriptor& descriptor, uint32 colorAttachmentIndex) const final override;

			ref<Image> createDepthStencilAttachmentImage(const AttachmentImageDescriptor& descriptor) const final override;

		public:
			const VulkanSurface* surface;

			VkSampleCountFlagBits sampleCountFlagBits;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
	};
}