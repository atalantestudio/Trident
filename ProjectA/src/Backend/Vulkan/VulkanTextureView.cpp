#include "pch.hpp"

#include "Backend/Vulkan/TextureFormat.hpp"
#include "Backend/Vulkan/TextureType.hpp"
#include "Backend/Vulkan/VulkanTexture.hpp"
#include "Backend/Vulkan/VulkanTextureView.hpp"

namespace ProjectA {
	VulkanTextureView::VulkanTextureView(VkDevice device, const VulkanTexture& texture, const TextureViewDescriptor& descriptor) :
		device(device)
	{
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = texture.object;
		imageViewCreateInfo.viewType = getVkImageViewType(texture.type);
		imageViewCreateInfo.format = getVkFormat(texture.format);
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create image view.");
		}
	}

	VulkanTextureView::~VulkanTextureView() {
		vkDestroyImageView(device, object, nullptr);
	}
}