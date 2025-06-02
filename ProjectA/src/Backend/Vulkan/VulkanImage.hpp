#pragma once

#include "Backend/Vulkan/MemoryAllocator.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class VulkanImage : public Image, public ObjectWrapper<VkImage> {
		public:
			VulkanImage(VkDevice device, const ImageDescriptor& descriptor);

			~VulkanImage();

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

		public:
			VkImageType type;
			VkFormat format;
			uint32 mipLevels;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
			uint32 width;
			uint32 height;
	};
}