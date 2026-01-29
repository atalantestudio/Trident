// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Image.hpp"

namespace Trident {
	class GL::Image final : public Trident::Image {
		public:
			explicit Image(const ImageDescriptor& descriptor);

			~Image() final override;

		protected:
			ref<Trident::ImageView> createViewInternal(const ImageViewDescriptor& descriptor, Trident::Image* image) final override;

		public:
			GLuint texture;
			GLenum target;
			ImageUsage usage;
			GLenum internalFormat;
			GLenum format;
	};
}