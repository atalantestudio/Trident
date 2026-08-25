// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ImageView.hpp"

namespace Trident {
	class GL::ImageView final : public Trident::ImageView {
		public:
			explicit ImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		public:
			GLuint texture;
			GLenum target;
	};

	namespace GL {
		ref<Trident::ImageView> createImageView(const Trident::Device* device, Trident::Image* image, const ImageViewDescriptor& descriptor);

		void destroyImageView(const Trident::Device* device, const Trident::ImageView* imageView);
	}
}