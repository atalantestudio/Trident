#include "pch.hpp"

#include "Platform/Vulkan/VulkanRenderPass.hpp"
#include "Platform/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	VulkanSwapchain::VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VkDevice device, const SwapchainDescriptor& descriptor) :
		device(device),
		imageExtent(surface.capabilities.maxImageExtent)
	{
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[VulkanSwapchain]");

		const uint32 minImageCount = surface.capabilities.minImageCount + 1;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface.object;
		swapchainCreateInfo.minImageCount = minImageCount;
		swapchainCreateInfo.imageFormat = surface.format.format;
		swapchainCreateInfo.imageColorSpace = surface.format.colorSpace;
		swapchainCreateInfo.imageExtent = imageExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (physicalDevice.graphicsQueueFamilyIndex == surface.presentQueueFamilyIndex) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			const uint32 queueFamilyIndices[] = {physicalDevice.graphicsQueueFamilyIndex, surface.presentQueueFamilyIndex};

			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}

		swapchainCreateInfo.preTransform = surface.capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = surface.presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &object) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create Vulkan swapchain.");
		}

		uint32 imageCount;

		vkGetSwapchainImagesKHR(device, object, &imageCount, nullptr);

		images.resize(imageCount);
		imageViews.resize(imageCount);
		framebuffers.resize(imageCount);

		vkGetSwapchainImagesKHR(device, object, &imageCount, images.data());

		for (uint32 i = 0; i < imageCount; i++) {
			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.pNext = nullptr;
			imageViewCreateInfo.flags = 0;
			imageViewCreateInfo.image = images[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = surface.format.format;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan swapchain image view.");
			}

			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = reinterpret_cast<const VulkanRenderPass*>(descriptor.renderPass.get())->object;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = &imageViews[i];
			framebufferCreateInfo.width = imageExtent.width;
			framebufferCreateInfo.height = imageExtent.height;
			framebufferCreateInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("Failed to create Vulkan swapchain framebuffer.");
			}
		}
	}

	VulkanSwapchain::~VulkanSwapchain() {
		PROFILE_SCOPE(FUNC_SIG);

		ENGINE_TRACE("[~VulkanSwapchain]");

		for (uint32 i = 0; i < images.size(); i++) {
			vkDestroyFramebuffer(device, framebuffers[i], nullptr);
			vkDestroyImageView(device, imageViews[i], nullptr);

			// Images are implicitly destroyed by vkDestroySwapchainKHR.
		}

		vkDestroySwapchainKHR(device, object, nullptr);
	}

	uint32 VulkanSwapchain::getExtentWidth() const {
		return imageExtent.width;
	}

	uint32 VulkanSwapchain::getExtentHeight() const {
		return imageExtent.height;
	}
}