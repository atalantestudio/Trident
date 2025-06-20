#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class OpenGLImage final : public Image, public ObjectWrapper<GLuint> {
		public:
			OpenGLImage(const ImageDescriptor& descriptor);

			~OpenGLImage() final override;

			ref<ImageView> createViewInternal(const Image* image, const ImageViewDescriptor& descriptor) const final override;

		public:
			const GLenum target;
			const ImageUsage usage;
			const GLenum internalFormat;
			const GLenum format;
	};
}