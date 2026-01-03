#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ImageViewDescriptor.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class VulkanImageView final : public ImageView, public ObjectWrapper<VkImageView> {
		public:
			explicit VulkanImageView(VkDevice device, const Image* image, const ImageViewDescriptor& descriptor);

			~VulkanImageView() final override;

		private:
			const VkDevice device;
	};
}