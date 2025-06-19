#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class OpenGLImageView : public ImageView, public ObjectWrapper<GLuint> {
		public:
			OpenGLImageView(const Image* image, const ImageViewDescriptor& descriptor);

			~OpenGLImageView();
	};
}