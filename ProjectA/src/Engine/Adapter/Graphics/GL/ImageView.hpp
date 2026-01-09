// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class GL::ImageView final : public ProjectA::ImageView {
		public:
			explicit ImageView(const ImageViewDescriptor& descriptor, const ProjectA::Image* image);

			~ImageView() final;

		public:
			GLuint texture;
	};
}