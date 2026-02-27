// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace Trident {
	class VK::ImageView final : public Trident::ImageView {
		public:
			explicit ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		public:
			VkImageView imageView;
	};

	namespace VK {
		ref<Trident::ImageView> createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		void destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView);
	}
}