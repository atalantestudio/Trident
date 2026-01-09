// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor, const VK::Device& device) :
		ProjectA::Framebuffer(descriptor),
		device(device)
	{
		sequence<VkImageView> imageViews(descriptor.imageViews.count());

		for (uint32 attachmentImageViewIndex = 0; attachmentImageViewIndex < descriptor.imageViews.count(); attachmentImageViewIndex += 1) {
			imageViews[attachmentImageViewIndex] = std::static_pointer_cast<VK::ImageView>(descriptor.imageViews[attachmentImageViewIndex])->imageView;
		}

		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = std::static_pointer_cast<VK::RenderPass>(descriptor.renderPass)->renderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32>(imageViews.count());
		framebufferCreateInfo.pAttachments = &imageViews[0];
		framebufferCreateInfo.width = descriptor.extent.width;
		framebufferCreateInfo.height = descriptor.extent.height;
		framebufferCreateInfo.layers = descriptor.layerCount;

		const VkResult result = vkCreateFramebuffer(device.device, &framebufferCreateInfo, nullptr, &framebuffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer ([]).", string_VkResult(result));
	}

	VK::Framebuffer::~Framebuffer() {
		vkDestroyFramebuffer(device.device, framebuffer, nullptr);
	}
}