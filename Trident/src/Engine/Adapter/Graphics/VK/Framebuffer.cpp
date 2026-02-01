// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Framebuffer::Framebuffer(const FramebufferDescriptor& descriptor, const VK::Device& device) :
		Trident::Framebuffer(descriptor),
		device(device)
	{
		VkResult result;
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

		result = vkCreateFramebuffer(device.device, &framebufferCreateInfo, nullptr, &framebuffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_FRAMEBUFFER;
				debugUtilsObjectNameInfo.objectHandle = (uint64) framebuffer;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(device.device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif
	}

	VK::Framebuffer::~Framebuffer() {
		vkDestroyFramebuffer(device.device, framebuffer, nullptr);
	}
}