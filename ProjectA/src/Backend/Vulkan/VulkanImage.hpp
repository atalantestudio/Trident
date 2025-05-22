#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class VulkanDevice;

	class VulkanImage : public Image, public ObjectWrapper<VkImage> {
		public:
			VulkanImage(const VulkanDevice& device, const ImageDescriptor& descriptor);

			VulkanImage(const VulkanDevice& device, const VkImageCreateInfo& createInfo);

			~VulkanImage();

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

		private:
			void construct(const VulkanDevice& device, const VkImageCreateInfo& createInfo);

		public:
			VkImageType type;
			VkFormat format;
			uint32 mipLevels;

		private:
			VkDevice device;
			VkDeviceMemory memory;
			uint32 width;
			uint32 height;
	};
}