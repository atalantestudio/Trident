// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#include "pch.hpp"

#include "Engine/Adapter/Graphics/VK/Image.hpp"
#include "Engine/Adapter/Graphics/VK/Swapchain.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident {
	VK::Swapchain::Swapchain(const SwapchainDescriptor& descriptor, const VK::Surface& surface, const VK::Device& device) :
		Trident::Swapchain(descriptor),
		swapchain{},
		surface(surface),
		device(device)
	{
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		recreate(descriptor);
	}

	VK::Swapchain::~Swapchain() {
		for (const ref<Trident::ImageView> imageView : imageViews) {
			imageView.~shared_ptr();
		}

		// The swapchain images are automatically destroyed with the swapchain.
		vkDestroySwapchainKHR(device.device, swapchain, nullptr);
	}

	uint32 VK::Swapchain::getImageCount() const {
		return imageCount;
	}

	void VK::Swapchain::update(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		for (const ref<Trident::ImageView> imageView : imageViews) {
			imageView.~shared_ptr();
		}

		recreate(descriptor);
	}

	// NOTE: ~59Mb in Release mode.
	void VK::Swapchain::recreate(const SwapchainDescriptor& descriptor) {
		DEBUG_DESCRIPTOR_GUARD(descriptor);

		VkResult result;

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat();

		ADAPTER_INFO("Swapchain format is []/[].", string_VkFormat(surfaceFormat.format), string_VkColorSpaceKHR(surfaceFormat.colorSpace));

		const VkPresentModeKHR presentMode = surface.selectPresentMode(descriptor.vSync);

		ADAPTER_INFO("Swapchain present mode is [].", string_VkPresentModeKHR(presentMode));

		const VkSurfaceCapabilitiesKHR surfaceCapabilities = surface.getCapabilities();

		imageCount = surfaceCapabilities.minImageCount + 1;

		if (surfaceCapabilities.maxImageCount > 0) {
			imageCount = min(imageCount, surfaceCapabilities.maxImageCount);
		}

		ADAPTER_INFO("Swapchain image count is [].", imageCount);

		if (surfaceCapabilities.currentExtent.width == max<uint32>() && surfaceCapabilities.currentExtent.height == max<uint32>()) {
			extent = *pointer_cast<const Extent2D*>(&surfaceCapabilities.currentExtent);
		}
		else {
			extent.width = clamp(descriptor.extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = clamp(descriptor.extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}

		ADAPTER_INFO("Swapchain image extent is ([], []).", extent.width, extent.height);

		const VkSwapchainKHR oldSwapchain = swapchain;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface.surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = *pointer_cast<const VkExtent2D*>(&extent);
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;

		result = vkCreateSwapchainKHR(device.device, &swapchainCreateInfo, nullptr, &swapchain);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to create swapchain ([]).", string_VkResult(result));

		vkDestroySwapchainKHR(device.device, oldSwapchain, nullptr);

		result = vkGetSwapchainImagesKHR(device.device, swapchain, &imageCount, nullptr);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get swapchain image count ([]).", string_VkResult(result));

		sequence<VkImage> vkImages(imageCount);

		result = vkGetSwapchainImagesKHR(device.device, swapchain, &imageCount, &vkImages[0]);

		ADAPTER_ASSERT(result == VK_SUCCESS, "Failed to get swapchain images ([]).", string_VkResult(result));

		images = sequence<ref<Trident::Image>>(imageCount);
		imageViews = sequence<ref<Trident::ImageView>>(imageCount);

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			images[imageIndex] = createRef<VK::Image>(device, vkImages[imageIndex], surfaceFormat.format, extent);
		}

		createImageViews();
	}
}