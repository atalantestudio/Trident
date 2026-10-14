// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#if defined(DEBUG_OBJECT_NAME)
	#include "Engine/Adapter/Graphics/VK/Debug/ObjectName.hpp"
#endif

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

		ref<VK::ImageView> imageView = allocateRef<VK::ImageView>(device, image, descriptor);

		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.image = _image->image;
		imageViewCreateInfo.viewType = getImageViewType(descriptor.type, descriptor.layerCount);
		imageViewCreateInfo.format = _image->format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = _image->aspectFlags;
		imageViewCreateInfo.subresourceRange.baseMipLevel = descriptor.mipLevelOffset;
		imageViewCreateInfo.subresourceRange.levelCount = descriptor.mipLevelCount;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = descriptor.layerOffset;
		imageViewCreateInfo.subresourceRange.layerCount = descriptor.layerCount;

		result = _device->vkCreateImageView(_device->device, &imageViewCreateInfo, nullptr, &imageView->imageView);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create image view ([]).", static_cast<sint32>(result));

		#if defined(DEBUG_OBJECT_NAME)
			setObjectName(_device, (uint64) imageView->imageView, VK_OBJECT_TYPE_IMAGE_VIEW, descriptor.name);
		#endif

		return imageView;
	}

	void VK::destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::ImageView* _imageView = static_cast<const VK::ImageView*>(imageView);

		_device->vkDestroyImageView(_device->device, _imageView->imageView, nullptr);
	}
}