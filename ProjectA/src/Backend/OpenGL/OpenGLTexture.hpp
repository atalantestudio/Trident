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

		private:
			const GLenum target;
			const GLsizei width;
			const GLsizei height;
	};
}