// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/ImageFormat.hpp"
#include "Engine/Adapter/Graphics/VK/ImageView.hpp"
#include "Engine/Adapter/Graphics/VK/ImageViewType.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::ImageView::ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) :
		Trident::ImageView(device, image, descriptor)
	{}

	ref<Trident::ImageView> VK::createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Image* _image = static_cast<const VK::Image*>(image);
		VkResult result;

		ref<VK::ImageView> imageView = createRef<VK::ImageView>(device, image, descriptor);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = _image->image;
		imageViewCreateInfo.viewType = getImageViewType(_image->type, image->getLayerCount());
		imageViewCreateInfo.format = _image->format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = 0;

		if (isDepthFormat(_image->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		if (isStencilFormat(_image->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}

		if (!isDepthFormat(_image->format) && !isStencilFormat(_image->format)) {
			imageViewCreateInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_COLOR_BIT;
		}

		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = image->getMipLevelCount();
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = image->getLayerCount();

		result = _device->vkCreateImageView(_device->device, &imageViewCreateInfo, nullptr, &imageView->imageView);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image view ([]).", string_VkResult(result));

		return imageView;
	}

	void VK::destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::ImageView* _imageView = static_cast<const VK::ImageView*>(imageView);

		_device->vkDestroyImageView(_device->device, _imageView->imageView, nullptr);
	}
}