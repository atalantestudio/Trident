#pragma once

#include <glad/glad.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"

namespace ProjectA {
	class OpenGLBuffer : public Buffer, public ObjectWrapper<GLuint> {
		public:
			OpenGLBuffer(const BufferDescriptor& descriptor);

			~OpenGLBuffer();

			void write(const void* data, const uint64 size) const override;

		public:
			const GLenum target;
			const GLsizeiptr size;
	};
}