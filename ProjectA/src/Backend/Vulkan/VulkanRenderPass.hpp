#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/RenderPass.hpp"

namespace ProjectA {
	class VulkanRenderPass : public RenderPass, public ObjectWrapper<VkRenderPass> {
		private:
			static void countAttachments(const RenderPassDescriptor& descriptor, uint32& colorAttachmentCount, uint32& resolveAttachmentCount, uint32& depthStencilAttachmentCount);

		public:
			VulkanRenderPass(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const RenderPassDescriptor& descriptor);

			~VulkanRenderPass();

		public:
			uint32 attachmentCount;
			const chain<Attachment> colorAttachments;
			Attachment depthStencilAttachment;
			uint32 surfaceExtentWidth;
			uint32 surfaceExtentHeight;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
	};
}