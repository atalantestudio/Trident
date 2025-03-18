#pragma once

#include <glad/glad.h>

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Texture.hpp"

// TODO: Old API
namespace ProjectA {
	class OpenGLTexture : public Texture, public ObjectWrapper<GLuint> {
		public:
			OpenGLTexture(GLsizei width, GLsizei height);

			~OpenGLTexture();

			uint32 getWidth() const override;
			uint32 getHeight() const override;

		private:
			GLenum internalFormat;
			GLenum format;
			GLsizei width;
			GLsizei height;
	};
}