#pragma once

#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class VulkanImage final : public Image, public ObjectWrapper<VkImage> {
		public:
			explicit VulkanImage(VkDevice device, const ImageDescriptor& descriptor);

			// NOTE: Use this constructor to create a wrapper around a swapchain image.
			explicit VulkanImage(VkDevice device, VkImage image, VkFormat format, Extent2D extent);

			~VulkanImage() final override;

			ref<ImageView> createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const final override;

		private:
			static ImageDescriptor createSwapchainImageDescriptor(Extent2D extent);

		public:
			VkImageType type;
			VkFormat format;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
			bool swapchainImage;
	};
}