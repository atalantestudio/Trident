// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Buffer.hpp"

namespace Trident {
	class VK::Buffer final : public Trident::Buffer {
		public:
			explicit Buffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		public:
			VkBuffer buffer;
			VkMemoryRequirements memoryRequirements;
	};

	namespace VK {
		ref<Trident::Buffer> createBuffer(const Trident::Device* device, const BufferDescriptor& descriptor);

		void destroyBuffer(const Trident::Device* device, const Trident::Buffer* buffer);

		void* mapBuffer(const Trident::Device* device, Trident::Buffer* buffer, uint64 offset, uint64 size);

		void unmapBuffer(const Trident::Device* device, Trident::Buffer* buffer);
	}
}