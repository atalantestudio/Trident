#pragma once

#include "ProjectA/Renderer/Texture2D.hpp"

namespace ProjectA {
	class OpenGLTexture2D : public Texture2D {
		private:
			uint32_t texture;
			uint32_t width;
			uint32_t height;

			std::string debugPath;

		public:
			OpenGLTexture2D(const std::string& path);
			virtual ~OpenGLTexture2D();

			virtual void bind(uint32_t slot) const override;

			virtual uint32_t getWidth() const override;
			virtual uint32_t getHeight() const override;
	};
}