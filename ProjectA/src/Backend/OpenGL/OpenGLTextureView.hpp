#pragma once

#include <glad/glad.h>

#include "Backend/OpenGL/OpenGLTexture.hpp"

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/TextureView.hpp"

namespace ProjectA {
	class OpenGLTextureView : public TextureView, public ObjectWrapper<GLuint> {
		public:
			OpenGLTextureView(const OpenGLTexture& texture, const TextureViewDescriptor& descriptor);

			~OpenGLTextureView();

		public:
			const GLenum texture;
	};
}