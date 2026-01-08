#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/ImageView.hpp"

namespace ProjectA {
	class OpenGLImageView final : public ImageView, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLImageView(const ImageViewDescriptor& descriptor, const Image* image);

			~OpenGLImageView() final;
	};
}