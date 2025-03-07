#pragma once

#include <glad/glad.h>

#include "Engine/ObjectWrapper.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"

namespace ProjectA {
	class OpenGLBuffer : public Buffer, public ObjectWrapper<GLuint> {
		public:
			OpenGLBuffer(const BufferDescriptor& descriptor);

			~OpenGLBuffer();

			void map(const uint64 offset, const uint64 size) override;

			void write(const void* data, const uint64 offset, const uint64 size) override;

			void unmap() override;

		public:
			const GLenum target;
			const GLsizeiptr size;
	};
}