// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	class GL::BaseBuffer final : public Trident::BaseBuffer {
		public:
			explicit BaseBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			GLuint buffer;

			#if OPENGL_VERSION < OPENGL_VERSION_4_5
				const GLenum target;
			#endif

			GLbitfield storageAccessFlags = 0;
			GLbitfield mapAccessFlags = 0;
	};

	namespace GL {
		ref<Trident::BaseBuffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor, UINT stride);

		void destroyBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer);

		void* mapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer);
	}
}