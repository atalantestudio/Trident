#pragma once

#include "Backend/Vulkan/VulkanDevice.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Image.hpp"

namespace ProjectA {
	class VulkanImage : public Image, public ObjectWrapper<VkImage> {
		public:
			VulkanImage(const VulkanDevice& device, const ImageDescriptor& descriptor);

			~VulkanImage();

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

		public:
			const ImageType type;
			const ImageFormat format;

		private:
			VkDevice device;
			VkDeviceMemory memory;
			const uint32 width;
			const uint32 height;
	};
}