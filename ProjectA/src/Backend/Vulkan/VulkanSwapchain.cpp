#include "pch.hpp"

#include "Backend/Vulkan/VulkanDevice.hpp"
#include "Backend/Vulkan/VulkanRenderPass.hpp"
#include "Backend/Vulkan/VulkanSwapchain.hpp"

#include "Backend/Vulkan/ImageFormat.hpp"

namespace ProjectA {
	VulkanSwapchain::VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const SwapchainDescriptor& descriptor) :
		physicalDevice(physicalDevice),
		device(device),
		surface(surface)
	{
		PROFILE_SCOPE(FUNC_SIG);

		construct(descriptor);
	}

	VulkanSwapchain::~VulkanSwapchain() {
		destroyImageViewsAndFramebuffers();
		destroySwapchain(object);
	}

	uint32 VulkanSwapchain::getImageCount() const {
		return imageCount;
	}

	void VulkanSwapchain::update(const SwapchainDescriptor& descriptor) {
		destroyImageViewsAndFramebuffers();

		construct(descriptor);
	}

	void VulkanSwapchain::getImages(const ref<RenderPass>& renderPass) {
		PROFILE_SCOPE(FUNC_SIG);

		VkResult result;

		result = vkGetSwapchainImagesKHR(device.object, object, &imageCount, nullptr);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain image count ([]).", string_VkResult(result));

		images.resize(imageCount);
		imageViews.resize(imageCount);
		framebuffers.resize(imageCount);

		result = vkGetSwapchainImagesKHR(device.object, object, &imageCount, images.data());

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to get swapchain images ([]).", string_VkResult(result));

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat(physicalDevice.object);

		// TODO: Replace with ImageView API.
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = surfaceFormat.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		const ref<VulkanRenderPass>& vkRenderPass = std::static_pointer_cast<VulkanRenderPass>(renderPass);

		const uint32 attachmentCount = renderPass->getAttachmentCount();
		const uint32 colorAttachmentCount = renderPass->getColorAttachmentCount();
		const uint32 resolveAttachmentCount = renderPass->getResolveAttachmentCount();
		const uint32 presentAttachmentIndex = renderPass->getPresentAttachmentIndex();

		// TODO: Replace with Framebuffer API.
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = vkRenderPass->object;
		framebufferCreateInfo.attachmentCount = attachmentCount;
		framebufferCreateInfo.width = extent.width;
		framebufferCreateInfo.height = extent.height;
		framebufferCreateInfo.layers = 1;

		const std::vector<ref<ImageView>>& colorAttachmentImageViews = renderPass->getColorAttachmentImageViews();

		std::vector<VkImageView> baseAttachmentsImageViews(attachmentCount);

		// Write color attachment image views (except for the presented attachment).
		for (uint32 colorAttachmentIndex = 0; colorAttachmentIndex < colorAttachmentCount; colorAttachmentIndex += 1) {
			if (resolveAttachmentCount == 0 && colorAttachmentIndex == presentAttachmentIndex) {
				continue;
			}

			const ref<ImageView>& colorAttachmentImageView = colorAttachmentImageViews[colorAttachmentIndex];

			baseAttachmentsImageViews[colorAttachmentIndex] = std::static_pointer_cast<VulkanImageView>(colorAttachmentImageView)->object;
		}

		// Write depth stencil attachment image view.
		if (renderPass->depthStencil) {
			const ref<ImageView>& depthStencilAttachmentImageView = renderPass->getDepthStencilAttachmentImageView();

			baseAttachmentsImageViews[colorAttachmentCount + resolveAttachmentCount] = std::static_pointer_cast<VulkanImageView>(depthStencilAttachmentImageView)->object;
		}

		for (uint32 imageIndex = 0; imageIndex < imageCount; imageIndex += 1) {
			imageViewCreateInfo.image = images[imageIndex];

			result = vkCreateImageView(device.object, &imageViewCreateInfo, nullptr, &imageViews[imageIndex]);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain image view ([]).", string_VkResult(result));

			std::vector<VkImageView> attachmentImageViews(baseAttachmentsImageViews);

			// Override eligible attachments (all resolve and present) with a swapchain image view.
			if (resolveAttachmentCount > 0) {
				// Every resolve attachment renders to a swapchain image view.
				for (uint32 resolveAttachmentIndex = 0; resolveAttachmentIndex < resolveAttachmentCount; resolveAttachmentIndex += 1) {
					attachmentImageViews[colorAttachmentCount + resolveAttachmentIndex] = imageViews[imageIndex];
				}
			}
			else {
				// If there is no resolve attachments, then 1 color attachment must render to a swapchain image view.
				DEBUG_ASSERT(presentAttachmentIndex != std::numeric_limits<uint32>::max());

				attachmentImageViews[presentAttachmentIndex] = imageViews[imageIndex];
			}

			framebufferCreateInfo.pAttachments = &attachmentImageViews[0];

			result = vkCreateFramebuffer(device.object, &framebufferCreateInfo, nullptr, &framebuffers[imageIndex]);

			ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan swapchain framebuffer ([]).", string_VkResult(result));
		}
	}

	// Seems to allocate ~59Mb of memory in Release mode.
	void VulkanSwapchain::construct(const SwapchainDescriptor& descriptor) {
		PROFILE_SCOPE(FUNC_SIG);

		const VkSurfaceFormatKHR surfaceFormat = surface.selectFormat(physicalDevice.object);

		ENGINE_INFO("Swapchain format is []/[].", string_VkFormat(surfaceFormat.format), string_VkColorSpaceKHR(surfaceFormat.colorSpace));

		const VkPresentModeKHR presentMode = surface.selectPresentMode(physicalDevice.object, descriptor.vSync);

		ENGINE_INFO("Swapchain present mode is [].", string_VkPresentModeKHR(presentMode));

		const VkSurfaceCapabilitiesKHR surfaceCapabilities = surface.getCapabilities(physicalDevice.object);

		uint32 imageCount = surfaceCapabilities.minImageCount + 1;

		if (surfaceCapabilities.maxImageCount > 0) {
			imageCount = std::min(imageCount, surfaceCapabilities.maxImageCount);
		}

		ENGINE_INFO("Swapchain image count is [].", imageCount);

		if (surfaceCapabilities.currentExtent.width == std::numeric_limits<uint32>::max() && surfaceCapabilities.currentExtent.height == std::numeric_limits<uint32>::max()) {
			extent = *pointer_cast_const<const Extent2D*>(&surfaceCapabilities.currentExtent);
		}
		else {
			extent.width = std::clamp(descriptor.extent.width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = std::clamp(descriptor.extent.height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
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
		swapchainCreateInfo.imageExtent = *pointer_cast_const<const VkExtent2D*>(&extent);
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
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = oldSwapchain;

		const VkResult result = vkCreateSwapchainKHR(device.object, &swapchainCreateInfo, nullptr, &object);

		ENGINE_ASSERT(result == VK_SUCCESS, "Failed to create swapchain ([]).", string_VkResult(result));

		destroySwapchain(oldSwapchain);
	}

	void VulkanSwapchain::destroyImageViewsAndFramebuffers() const {
		for (uint32 imageIndex = 0; imageIndex < images.size(); imageIndex += 1) {
			vkDestroyFramebuffer(device.object, framebuffers[imageIndex], nullptr);
			vkDestroyImageView(device.object, imageViews[imageIndex], nullptr);
		}
	}

	void VulkanSwapchain::destroySwapchain(VkSwapchainKHR swapchain) const {
		// The swapchain images are automatically destroyed with the swapchain.
		vkDestroySwapchainKHR(device.object, swapchain, nullptr);
	}
}