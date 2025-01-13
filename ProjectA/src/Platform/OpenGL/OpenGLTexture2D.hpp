#pragma once

#include <glad/glad.h>

#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	class OpenGLTexture2D : public Texture2D {
		private:
			uint32_t texture;
			GLenum internalFormat;
			GLenum format;
			GLsizei width;
			GLsizei height;

			std::string debugPath;

		public:
			OpenGLTexture2D(GLsizei width, GLsizei height);
			OpenGLTexture2D(const std::string& path);
			virtual ~OpenGLTexture2D();

			virtual void bind(uint32_t slot) const override;
			virtual void unbind() const override;

			virtual uint32 getWidth() const override;
			virtual uint32 getHeight() const override;

			virtual void setData(const void* data) const override;
	};
}