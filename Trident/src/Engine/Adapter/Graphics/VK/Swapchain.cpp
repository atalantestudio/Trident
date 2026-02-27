// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/PhysicalDevice.hpp"
#include "Engine/Adapter/Graphics/VK/Surface.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Swapchain::Swapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) :
		Trident::Swapchain(device, descriptor)
	{}

	ref<Trident::Swapchain> VK::createSwapchain(const Trident::Device* device, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		ref<VK::Swapchain> swapchain = createRef<VK::Swapchain>(device, descriptor);

		updateSwapchain(device, swapchain.get(), descriptor);

		return swapchain;
	}

	void VK::destroySwapchain(const Trident::Device* device, const Trident::Swapchain* swapchain) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		const VK::Swapchain* _swapchain = static_cast<const VK::Swapchain*>(swapchain);

		for (const ref<Trident::ImageView> imageView : _swapchain->imageViews) {
			imageView.~shared_ptr();
		}

		// The swapchain images are automatically destroyed with the swapchain.
		_device->vkDestroySwapchainKHR(_device->device, _swapchain->swapchain, nullptr);
	}

	void VK::recreateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<Trident::ImageView> imageView : swapchain->imageViews) {
			imageView.~shared_ptr();
		}

		updateSwapchain(device, swapchain, descriptor);
	}

	void VK::updateSwapchain(const Trident::Device* device, Trident::Swapchain* swapchain, const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		const VK::Device* _device = static_cast<const VK::Device*>(device);
		VK::Swapchain* _swapchain = static_cast<VK::Swapchain*>(swapchain);
		const ref<VK::Surface> surface = std::static_pointer_cast<VK::Surface>(descriptor.surface);
		VkResult result;

		_swapchain->imageCount = surface->capabilities.minImageCount + 1;

		if (surface->capabilities.maxImageCount > 0) {
			_swapchain->imageCount = min(_swapchain->imageCount, surface->capabilities.maxImageCount);
		}

		ADAPTER_INFO("Swapchain image count is [].", _swapchain->imageCount);

		if (surface->capabilities.currentExtent.width == max<uint32>() && surface->capabilities.currentExtent.height == max<uint32>()) {
			_swapchain->extent = *pointer_cast<const Extent2D*>(&surface->capabilities.currentExtent);
		}
		else {
			_swapchain->extent.width = clamp(descriptor.extent.width, surface->capabilities.minImageExtent.width, surface->capabilities.maxImageExtent.width);
			_swapchain->extent.height = clamp(descriptor.extent.height, surface->capabilities.minImageExtent.height, surface->capabilities.maxImageExtent.height);
		}

		ADAPTER_INFO("Swapchain image extent is ([], []).", _swapchain->extent.width, _swapchain->extent.height);

		const VkSwapchainKHR oldSwapchain = _swapchain->swapchain;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface->surface;
		swapchainCreateInfo.minImageCount = _swapchain->imageCount;
		swapchainCreateInfo.imageFormat = surface->format.format;
		swapchainCreateInfo.imageColorSpace = surface->format.colorSpace;
		swapchainCreateInfo.imageExtent = *pointer_cast<const VkExtent2D*>(&_swapchain->extent);
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = surface->capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		if (descriptor.verticalSync) {
			swapchainCreateInfo.presentMode = surface->synchronizedPresentMode;
		}
		else {
			swapchainCreateInfo.presentMode = surface->immediatePresentMode;
		}

		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;

		result = _device->vkCreateSwapchainKHR(_device->device, &swapchainCreateInfo, nullptr, &_swapchain->swapchain);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create swapchain ([]).", string_VkResult(result));

		_device->vkDestroySwapchainKHR(_device->device, oldSwapchain, nullptr);

		result = _device->vkGetSwapchainImagesKHR(_device->device, _swapchain->swapchain, &_swapchain->imageCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get swapchain image count ([]).", string_VkResult(result));

		sequence<VkImage> images(_swapchain->imageCount);

		result = _device->vkGetSwapchainImagesKHR(_device->device, _swapchain->swapchain, &_swapchain->imageCount, &images[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get swapchain images ([]).", string_VkResult(result));

		swapchain->images = sequence<ref<Trident::Image>>(_swapchain->imageCount);
		swapchain->imageViews = sequence<ref<Trident::ImageView>>(_swapchain->imageCount);

		uint32 imageIndex = 0;

		while (imageIndex < _swapchain->imageCount) {
			swapchain->images[imageIndex] = createImage(device, images[imageIndex], surface->format.format, _swapchain->extent, imageIndex);

			imageIndex += 1;
		}

		swapchain->createImageViews();
	}
}