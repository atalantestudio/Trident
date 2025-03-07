#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Texture2D.hpp"

// TODO: Old API
namespace ProjectA {
	class OpenGLTexture2D : public Texture2D {
		public:
			OpenGLTexture2D(GLsizei width, GLsizei height);
			OpenGLTexture2D(const std::string& path);

			~OpenGLTexture2D();

			uint32 getId() const override;

			uint32 getWidth() const override;
			uint32 getHeight() const override;

			void setData(const void* data, uint32 size) const override;

			void bind(uint32 slot) const override;
			void unbind() const override;

			bool operator ==(const Texture& other) const override;

		private:
			uint32 object;
			GLsizei width;
			GLsizei height;
			GLenum internalFormat;
			GLenum format;

			std::string debugPath;
	};
}