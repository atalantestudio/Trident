#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/Renderer/Buffer/Buffer.hpp"

namespace ProjectA {
	class OpenGLBuffer : public Buffer, public ObjectWrapper<GLuint> {
		public:
			OpenGLBuffer(const BufferDescriptor& descriptor);

			~OpenGLBuffer();

			void unmap() const override;

			void* getMappedRange(uint64 offset, uint64 size) const override;

		private:
			GLbitfield access;
	};
}