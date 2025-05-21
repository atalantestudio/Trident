#pragma once

#include "Backend/Vulkan/VulkanSurface.hpp"
#include "Backend/Vulkan/VulkanImageView.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Swapchain.hpp"

namespace ProjectA {
	class VulkanSwapchain : public Swapchain, public ObjectWrapper<VkSwapchainKHR> {
		public:
			VulkanSwapchain(const VulkanPhysicalDevice& physicalDevice, const VulkanSurface& surface, const VulkanDevice& device, const SwapchainDescriptor& descriptor);

			~VulkanSwapchain() final override;

			void recreate(const SwapchainDescriptor& descriptor) final override;

			uint32 getExtentWidth() const final override;
			uint32 getExtentHeight() const final override;

			void construct(const SwapchainDescriptor& descriptor);

			void destroy();

		private:
			void createColorImageAndImageView(Attachment colorAttachment, uint32 attachmentIndex);

			void createDepthStencilImageAndImageView(Attachment depthStencilAttachment);

		public:
			chain<VkImage> images;
			chain<VkImageView> imageViews;
			chain<VkFramebuffer> framebuffers;

		private:
			const VulkanPhysicalDevice& physicalDevice;
			const VulkanDevice& device;
			const VulkanSurface& surface;

			chain<scope<VulkanImage>> colorImages;
			chain<scope<VulkanImageView>> colorImageViews;

			scope<VulkanImage> depthStencilImage;
			scope<VulkanImageView> depthStencilImageView;
	};
}