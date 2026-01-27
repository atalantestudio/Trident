// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	class GL::Buffer final : public Trident::Buffer {
		public:
			explicit Buffer(const BufferDescriptor& descriptor);

			~Buffer() final override;

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		public:
			GLuint buffer;

		private:
			#if OPENGL_VERSION < OPENGL_VERSION_4_5
				const GLenum target;
			#endif

			GLbitfield storageAccessFlags = 0;
			GLbitfield mapAccessFlags = 0;
	};
}