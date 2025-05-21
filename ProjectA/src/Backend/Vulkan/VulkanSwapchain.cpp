#include "pch.hpp"

#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

namespace ProjectA {
	VulkanSwapchain::VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, VkDevice device, const SwapchainDescriptor& descriptor) :
		physicalDevice(physicalDevice),
		device(device),
		surface(surface)
	{
		PROFILE_SCOPE(FUNC_SIG);

		construct(descriptor);
	}

	VulkanSwapchain::~VulkanSwapchain() {
		PROFILE_SCOPE(FUNC_SIG);

		destroy();
	}

	void VulkanSwapchain::recreate(const SwapchainDescriptor& descriptor) {
		destroy();
		construct(descriptor);
	}

	uint32 VulkanSwapchain::getExtentWidth() const {
		return surface.window.getSize()[0];
	}

	uint32 VulkanSwapchain::getExtentHeight() const {
		return surface.window.getSize()[1];
	}

	// Seems to allocate ~59Mb of memory in Release mode.
	void VulkanSwapchain::construct(const SwapchainDescriptor& descriptor) {
		const VkSurfaceCapabilitiesKHR surfaceCapabilities = surface.getCapabilities();
		const uint32 minImageCount = surfaceCapabilities.minImageCount + 1;

		VkSwapchainCreateInfoKHR swapchainCreateInfo{};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = surface.object;
		swapchainCreateInfo.minImageCount = minImageCount;
		swapchainCreateInfo.imageFormat = surface.format.format;
		swapchainCreateInfo.imageColorSpace = surface.format.colorSpace;
		swapchainCreateInfo.imageExtent.width = descriptor.extentWidth;
		swapchainCreateInfo.imageExtent.height = descriptor.extentHeight;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		if (physicalDevice.graphicsQueueFamilyIndex == surface.presentQueueFamilyIndex) {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchainCreateInfo.queueFamilyIndexCount = 0;
			swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		}
		else {
			const uint32 queueFamilyIndices[2] = {physicalDevice.graphicsQueueFamilyIndex, surface.presentQueueFamilyIndex};

			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}

		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = surface.presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create swapchain (code: []).", result);

		uint32 imageCount;

		result = vkGetSwapchainImagesKHR(device, object, &imageCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain image count (code: []).", result);

		images.resize(imageCount);
		imageViews.resize(imageCount);
		framebuffers.resize(imageCount);

		result = vkGetSwapchainImagesKHR(device, object, &imageCount, images.data());

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain images (code: []).", result);

		VulkanRenderPass& renderPass = *std::static_pointer_cast<VulkanRenderPass>(descriptor.renderPass);

		renderPass.recreateResolveAttachmentImageViews(surface);
		renderPass.recreateDepthStencilAttachmentImageView(surface);

		for (uint32 i = 0; i < imageCount; i += 1) {
			// TODO: Replace with Image and ImageView APIs.
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

			result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain image view (code: []).", result);

			fchain<VkImageView> attachments(renderPass.attachmentCount);

			uint32 attachmentIndex = 0;

			for (; attachmentIndex < attachments.count - 1; attachmentIndex += 1) {
				attachments[attachmentIndex] = renderPass.getAttachmentImageView(attachmentIndex);
			}

			attachments[attachmentIndex] = imageViews[i];

			// TODO: Replace with Framebuffer API.
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.pNext = nullptr;
			framebufferCreateInfo.flags = 0;
			framebufferCreateInfo.renderPass = renderPass.object;
			framebufferCreateInfo.attachmentCount = renderPass.attachmentCount;
			framebufferCreateInfo.pAttachments = &attachments[0];
			framebufferCreateInfo.width = descriptor.extentWidth;
			framebufferCreateInfo.height = descriptor.extentHeight;
			framebufferCreateInfo.layers = 1;

			result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffers[i]);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain framebuffer (code: []).", result);
		}
	}

	void VulkanSwapchain::destroy() {
		for (uint32 imageIndex = 0; imageIndex < images.size(); imageIndex += 1) {
			vkDestroyFramebuffer(device, framebuffers[imageIndex], nullptr);
			vkDestroyImageView(device, imageViews[imageIndex], nullptr);
		}

		// The swapchain images are automatically destroyed with the swapchain.
		vkDestroySwapchainKHR(device, object, nullptr);
	}
}