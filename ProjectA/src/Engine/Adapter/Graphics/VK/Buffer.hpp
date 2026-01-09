// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/Adapter/Graphics/VK/MemoryAllocator.hpp"
#include "Engine/API/Rendering/Buffer.hpp"

namespace ProjectA {
	class VK::Buffer final : public ProjectA::Buffer {
		public:
			explicit Buffer(const BufferDescriptor& descriptor, const VK::Device& device);

			~Buffer() final override;

			void unmap() const final override;

			void* getMappedRange(uint64 offset, uint64 size) const final override;

		public:
			VkBuffer buffer;

		private:
			const VK::Device& device;
			MemorySuballocation memorySuballocation;
	};
}