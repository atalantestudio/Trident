#pragma once

#include "Backend/Vulkan/VulkanImage.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class VulkanImageView : public ImageView, public ObjectWrapper<VkImageView> {
		public:
			VulkanImageView(VkDevice device, const VulkanImage& image, const ImageViewDescriptor& descriptor);

			~VulkanImageView();

		private:
			const VkDevice device;
	};
}