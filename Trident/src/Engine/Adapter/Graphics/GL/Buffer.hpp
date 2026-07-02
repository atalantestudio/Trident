// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"
#include "Engine/API/Graphics/Device.hpp"

namespace Trident {
	class GL::Buffer final : public Trident::Buffer {
		public:
			explicit Buffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			GLuint buffer;
			GLenum target;
			GLbitfield storageAccessFlags = 0;
			GLbitfield mapAccessFlags = 0;
			void* mappedRange = nullptr;
	};

	namespace GL {
		ref<Trident::Buffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		void destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer);

		void* mapBuffer(const Trident::Device* device, Trident::Buffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, Trident::Buffer* buffer);
	}
}