#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class OpenGLImage final : public Image, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLImage(const ImageDescriptor& descriptor);

			~OpenGLImage() final override;

		protected:
			ref<ImageView> createViewInternal(const ImageViewDescriptor& descriptor, const Image* image) const final override;

		public:
			const GLenum target;
			const ImageUsage usage;
			const GLenum internalFormat;
			const GLenum format;
	};
}