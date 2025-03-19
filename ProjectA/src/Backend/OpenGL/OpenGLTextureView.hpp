#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/TextureView.hpp"

namespace ProjectA {
	class OpenGLTextureView : public TextureView {
		public:
			OpenGLTextureView(const TextureViewDescriptor& descriptor);

			~OpenGLTextureView();
	};
}