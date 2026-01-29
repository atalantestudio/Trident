// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace Trident {
	class VK::ImageView final : public Trident::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, Trident::Image* image, const VK::Device& device);

			~ImageView() final override;

		public:
			VkImageView imageView;

		private:
			const VK::Device& device;
	};
}