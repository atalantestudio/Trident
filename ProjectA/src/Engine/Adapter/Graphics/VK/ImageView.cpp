#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/ImageType.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"

namespace ProjectA {
	VulkanImageView::VulkanImageView(const ImageViewDescriptor& descriptor, const Image* image, VkDevice device) :
		ImageView(descriptor, image),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VulkanImage* vkImage = static_cast<const VulkanImage*>(image);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = vkImage->object;
		imageViewCreateInfo.viewType = getVkImageViewType(vkImage->type, image->getLayerCount());
		imageViewCreateInfo.format = vkImage->format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = 0;

		if (isDepthFormat(vkImage->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		if (isStencilFormat(vkImage->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		if (!isDepthFormat(vkImage->format) && !isStencilFormat(vkImage->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = image->getMipLevelCount();
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = image->getLayerCount();

		const VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan image view ([]).", string_VkResult(result));
	}

	VulkanImageView::~VulkanImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyImageView(device, object, nullptr);
	}
}