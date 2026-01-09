// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/ImageViewType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace ProjectA {
	VK::ImageView::ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image, const VK::Device& device) :
		ProjectA::ImageView(descriptor, image),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Image* vkImage = static_cast<const VK::Image*>(image);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = vkImage->image;
		imageViewCreateInfo.viewType = getImageViewType(vkImage->type, image->getLayerCount());
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

		const VkResult result = vkCreateImageView(device.device, &imageViewCreateInfo, nullptr, &imageView);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image view ([]).", string_VkResult(result));
	}

	VK::ImageView::~ImageView() {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		vkDestroyImageView(device.device, imageView, nullptr);
	}
}