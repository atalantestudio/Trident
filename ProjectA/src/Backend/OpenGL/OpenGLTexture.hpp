#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Texture.hpp"

namespace ProjectA {
	class OpenGLTexture : public Texture, public ObjectWrapper<GLuint> {
		public:
			OpenGLTexture(const TextureDescriptor& descriptor);

			~OpenGLTexture();

			uint32 getWidth() const override;

			uint32 getHeight() const override;

			ref<TextureView> createView(const TextureViewDescriptor& descriptor) const override;

		public:
			const GLenum target;

		private:
			const GLsizei width;
			const GLsizei height;
	};
}