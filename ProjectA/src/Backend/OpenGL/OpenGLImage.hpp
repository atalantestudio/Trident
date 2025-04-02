#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Image.hpp"

namespace ProjectA {
	class OpenGLImage : public Image, public ObjectWrapper<GLuint> {
		public:
			OpenGLImage(const ImageDescriptor& descriptor);

			~OpenGLImage();

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<ImageView> createView(const ImageViewDescriptor& descriptor) const override;

		public:
			const GLenum target;
			const GLenum format;

		private:
			const GLsizei width;
			const GLsizei height;
	};
}