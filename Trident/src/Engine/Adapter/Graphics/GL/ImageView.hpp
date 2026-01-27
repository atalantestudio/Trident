// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageView.hpp"

namespace Trident {
	class GL::ImageView final : public Trident::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, const Trident::Image* image);

			~ImageView() final;

		public:
			GLuint texture;
	};
}