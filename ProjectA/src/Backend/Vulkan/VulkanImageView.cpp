#include "pch.hpp"

#include "Backend/Vulkan/VulkanImage.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"
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
		imageViewCreateInfo.viewType = getVkImageViewType(image.type, image.getLayerCount());
		imageViewCreateInfo.format = image.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		if (isDepthFormat(image.format)) {
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}
		else {
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = image.getMipLevelCount();
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = image.getLayerCount();

		const VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view ([]).", string_VkResult(result));
	}

	VulkanImageView::~VulkanImageView() {
		PROFILE_SCOPE(FUNC_SIG);

		vkDestroyImageView(device, object, nullptr);
	}
}