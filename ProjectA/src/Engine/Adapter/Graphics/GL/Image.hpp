// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class GL::Image final : public ProjectA::Image {
		public:
			explicit Image(const ImageDescriptor& descriptor);

			~Image() final override;

		protected:
			ref<ProjectA::ImageView> createViewInternal(const ImageViewDescriptor& descriptor, const ProjectA::Image* image) const final override;

		public:
			GLuint texture;
			GLenum target;
			ImageUsage usage;
			GLenum internalFormat;
			GLenum format;
	};
}