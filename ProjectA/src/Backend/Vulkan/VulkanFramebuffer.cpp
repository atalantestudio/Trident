#include "pch.hpp"

#include "Backend/Vulkan/VulkanFramebuffer.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"

namespace ProjectA {
	VulkanFramebuffer::VulkanFramebuffer(VkDevice device, const FramebufferDescriptor& descriptor) :
		Framebuffer(descriptor),
		device(device)
	{
		sequence<VkImageView> imageViews(descriptor.imageViews.count());

		for (uint32 attachmentImageViewIndex = 0; attachmentImageViewIndex < descriptor.imageViews.count(); attachmentImageViewIndex += 1) {
			imageViews[attachmentImageViewIndex] = std::static_pointer_cast<VulkanImageView>(descriptor.imageViews[attachmentImageViewIndex])->object;
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = std::static_pointer_cast<VulkanRenderPass>(descriptor.renderPass)->object;
		framebufferCreateInfo.attachmentCount = static_cast<uint32>(imageViews.count());
		framebufferCreateInfo.pAttachments = &imageViews[0];
		framebufferCreateInfo.width = descriptor.extent.width;
		framebufferCreateInfo.height = descriptor.extent.height;
		framebufferCreateInfo.layers = descriptor.layerCount;

		const VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan framebuffer ([]).", string_VkResult(result));
	}

	VulkanFramebuffer::~VulkanFramebuffer() {
		vkDestroyFramebuffer(device, object, nullptr);
	}
}