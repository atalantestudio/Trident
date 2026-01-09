// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/Device.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class VK::ImageView final : public ProjectA::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image, const VK::Device& device);

			~ImageView() final override;

		public:
			VkImageView imageView;

		private:
			const VK::Device& device;
	};
}