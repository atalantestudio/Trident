// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Image.hpp"

namespace Trident {
	class GL::Image final : public Trident::Image {
		public:
			explicit Image(const Trident::Device* device, const ImageDescriptor& descriptor);

		public:
			GLuint texture;
			GLenum target;
			ImageUsage usage;
			GLenum internalFormat;
			GLenum format;
	};

	namespace GL {
		ref<Trident::Image> createImage(const Trident::Device* device, const ImageDescriptor& descriptor);
		ref<Trident::Image> createImage(const Trident::Device* device, VkImage image, VkFormat format, Extent2D extent, uint32 index);

		void destroyImage(const Trident::Device* device, const Trident::Image* image);
	}
}