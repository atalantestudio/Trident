#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Texture2D.hpp"

namespace ProjectA {
	class OpenGLTexture2D : public Texture2D {
		public:
			OpenGLTexture2D(GLsizei width, GLsizei height);
			OpenGLTexture2D(const std::string& path);

			virtual ~OpenGLTexture2D();

			virtual uint32 getId() const override;

			virtual uint32 getWidth() const override;
			virtual uint32 getHeight() const override;

			virtual void setData(const void* data, uint32 size) const override;

			virtual void bind(uint32 slot) const override;
			virtual void unbind() const override;

			virtual bool operator ==(const Texture& other) const override;

		private:
			uint32 texture;
			GLsizei width;
			GLsizei height;
			GLenum internalFormat;
			GLenum format;

			std::string debugPath;
	};
}