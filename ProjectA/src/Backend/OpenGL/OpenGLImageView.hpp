#pragma once

#include "Backend/OpenGL/OpenGLImage.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class OpenGLImageView : public ImageView, public ObjectWrapper<GLuint> {
		public:
			OpenGLImageView(const OpenGLImage& image, const ImageViewDescriptor& descriptor);

			~OpenGLImageView();

		public:
			const OpenGLImage& image;
	};
}