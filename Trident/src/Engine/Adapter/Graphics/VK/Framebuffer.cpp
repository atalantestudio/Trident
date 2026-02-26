// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Framebuffer.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/RenderPass.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Framebuffer::Framebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) :
		Trident::Framebuffer(device, descriptor)
	{}

	ref<Trident::Framebuffer> VK::createFramebuffer(const Trident::Device* device, const FramebufferDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VkResult result;

		ref<VK::Framebuffer> framebuffer = createRef<VK::Framebuffer>(device, descriptor);

		sequence<VkImageView> imageViews(descriptor.imageViews.count());
		uint32 attachmentImageViewIndex = 0;

		while (attachmentImageViewIndex < descriptor.imageViews.count()) {
			imageViews[attachmentImageViewIndex] = std::static_pointer_cast<VK::ImageView>(descriptor.imageViews[attachmentImageViewIndex])->imageView;

			attachmentImageViewIndex += 1;
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
		framebufferCreateInfo.layers = 1;

		result = _device->vkCreateFramebuffer(_device->device, &framebufferCreateInfo, nullptr, &framebuffer->framebuffer);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer ([]).", string_VkResult(result));

		#if defined(BUILD_MODE_DEBUG)
			if (descriptor.name.count() > 0) {
				VkDebugUtilsObjectNameInfoEXT debugUtilsObjectNameInfo{};
				debugUtilsObjectNameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
				debugUtilsObjectNameInfo.pNext = nullptr;
				debugUtilsObjectNameInfo.objectType = VK_OBJECT_TYPE_FRAMEBUFFER;
				debugUtilsObjectNameInfo.objectHandle = (uint64) framebuffer->framebuffer;
				debugUtilsObjectNameInfo.pObjectName = &descriptor.name[0];

				result = vkSetDebugUtilsObjectNameEXT(_device->device, &debugUtilsObjectNameInfo);

				ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to set object name ([]).", string_VkResult(result));
			}
		#endif

		return framebuffer;
	}

	void VK::destroyFramebuffer(const Trident::Device* device, const Trident::Framebuffer* framebuffer) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Framebuffer* _framebuffer = static_cast<const VK::Framebuffer*>(framebuffer);

		_device->vkDestroyFramebuffer(_device->device, _framebuffer->framebuffer, nullptr);
	}
}