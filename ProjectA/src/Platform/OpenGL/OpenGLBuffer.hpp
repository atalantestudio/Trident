#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Buffer/Buffer.hpp"

namespace ProjectA {
	class OpenGLBuffer : public Buffer {
		public:
			OpenGLBuffer(const BufferDescriptor& descriptor);

			~OpenGLBuffer();

			void write(const void* data) const override;

		private:
			GLuint object;
			const GLsizeiptr size;
			GLenum binding;
	};
}