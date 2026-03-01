// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace Trident {
	class VK::BaseBuffer final : public Trident::BaseBuffer {
		public:
			explicit BaseBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			VkBuffer buffer;
			MemorySuballocation memorySuballocation;
	};

	namespace VK {
		ref<Trident::BaseBuffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor, UINT stride);

		void destroyBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer);

		void* mapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, const Trident::BaseBuffer* buffer);
	}
}