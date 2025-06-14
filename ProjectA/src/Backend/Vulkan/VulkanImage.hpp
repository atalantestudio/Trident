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

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

			void setLayout(ImageLayout layout);

		public:
			VkImageType type;
			VkFormat format;

		private:
			VkDevice device;
			MemorySuballocation memorySuballocation;
	};
}