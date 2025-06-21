#pragma once

#include "Engine/API/ObjectWrapper.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class OpenGLBuffer final : public Buffer, public ObjectWrapper<GLuint> {
		public:
			explicit OpenGLBuffer(const BufferDescriptor& descriptor);

			~OpenGLBuffer() final override;

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		private:
			GLbitfield storageAccessFlags = 0;
			GLbitfield mapAccessFlags = 0;
	};
}