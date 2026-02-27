// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	class VK::Buffer final : public Trident::Buffer {
		public:
			explicit Buffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			VkBuffer buffer;
			MemorySuballocation memorySuballocation;
	};

	namespace VK {
		ref<Trident::Buffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor, UINT stride);

		void destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer);

		void* mapBuffer(const Trident::Device* device, const Trident::Buffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, const Trident::Buffer* buffer);
	}
}