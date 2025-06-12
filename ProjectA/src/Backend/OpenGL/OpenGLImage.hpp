#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Descriptor/ImageDescriptor.hpp"
#include "Engine/API/Rendering/Image.hpp"

namespace ProjectA {
	class OpenGLImage : public Image, public ObjectWrapper<GLuint> {
		public:
			OpenGLImage(const ImageDescriptor& descriptor);

			~OpenGLImage();

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

		public:
			const GLenum target;
			const ImageUsage usage;
			const GLenum internalFormat;
			const GLenum format;
			const GLuint mipLevelCount;
			const GLuint layerCount;
	};
}