#include "pch.hpp"

#include "Backend/VK/Device.hpp"
#include "Backend/VK/Image.hpp"
#include "Backend/VK/Swapchain.hpp"

namespace ProjectA {
	VulkanSwapchain::VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const SwapchainDescriptor& descriptor) :
		Swapchain(descriptor),
		physicalDevice(physicalDevice),
		device(device),
		surface(surface)
	{
		updateInternal(descriptor);
	}

	VulkanSwapchain::~VulkanSwapchain() {
		for (const ref<ImageView>& imageView : imageViews) {
			imageView.~shared_ptr();
		}

		// The swapchain images are automatically destroyed with the swapchain.
		vkDestroySwapchainKHR(device.object, object, nullptr);
	}

	uint32 VulkanSwapchain::getImageCount() const {
		return imageCount;
	}

	void VulkanSwapchain::update(const SwapchainDescriptor& descriptor) {
		for (const ref<ImageView>& imageView : imageViews) {
			imageView.~shared_ptr();
		}

		updateInternal(descriptor);
	}

	// NOTE: ~59Mb in Release mode.
	void VulkanSwapchain::updateInternal(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNCTION_SIGNATURE);

		DEBUG_DESCRIPTOR_GUARD(descriptor);

		VkResult result;

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat();

		ENGINE_INFO("Swapchain format is []/[].", string_VkFormat(surfaceFormat.format), string_VkColorSpaceKHR(surfaceFormat.colorSpace));

		const VkPresentModeKHR presentMode = surface.selectPresentMode(descriptor.vSync);

		ENGINE_INFO("Swapchain present mode is [].", string_VkPresentModeKHR(presentMode));

		const VkSurfaceCapabilitiesKHR surfaceCapabilities = surface.getCapabilities();

		imageCount = surfaceCapabilities.minImageCount + 1;

		if (surfaceCapabilities.maxImageCount > 0) {
			imageCount = min(imageCount, surfaceCapabilities.maxImageCount);
		}

		ENGINE_INFO("Swapchain image count is [].", imageCount);

		if (surfaceCapabilities.currentExtent.width == max<uint32>() && surfaceCapabilities.currentExtent.height == max<uint32>()) {
			extent = *pointer_cast<const Extent2D*>(&surfaceCapabilities.currentExtent);
		}
		else {
			extent.width = clamp(descriptor.extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = clamp(descriptor.extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}

		ENGINE_INFO("Swapchain image extent is ([], []).", extent.width, extent.height);

		const VkSwapchainKHR oldSwapchain = object;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface.object;
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

		result = vkCreateSwapchainKHR(device.object, &swapchainCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create swapchain ([]).", string_VkResult(result));

		vkDestroySwapchainKHR(device.object, oldSwapchain, nullptr);

		result = vkGetSwapchainImagesKHR(device.object, object, &imageCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain image count ([]).", string_VkResult(result));

		sequence<VkImage> vkImages(imageCount);

		result = vkGetSwapchainImagesKHR(device.object, object, &imageCount, &vkImages[0]);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain images ([]).", string_VkResult(result));

		images = sequence<ref<Image>>(imageCount);

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			images[imageIndex] = createRef<VulkanImage>(device.object, vkImages[imageIndex], surfaceFormat.format, extent);
		}

		imageViews = sequence<ref<ImageView>>(imageCount);

		createImageViews();
	}
}