#include "pch.hpp"

#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Backend/Vulkan/ImageType.hpp"

namespace ProjectA {
	VulkanImageView::VulkanImageView(VkDevice device, const VulkanImage& image, const ImageViewDescriptor& descriptor) :
		device(device)
	{
		PROFILE_SCOPE(FUNC_SIG);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = image.object;
		imageViewCreateInfo.viewType = getVkImageViewType(image.type);
		imageViewCreateInfo.format = image.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = image.mipLevels;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		const VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view (code: []).", result);
	}

	VulkanImageView::~VulkanImageView() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyImageView(device, object, nullptr);
	}
}