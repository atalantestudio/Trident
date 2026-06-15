// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocation.hpp"

namespace Trident {
	class MemoryAllocator {
		private:
			static constexpr uint64 DYNAMIC_MEMORY_ALLOCATION_SIZE = 16000000;

		public:
			/// Creates a memory allocation and returns its index.
			uint32 createAllocation(uint32 memoryTypeIndex, uint64 size);

			// TODO
			// void suballocateBuffer();

			// TODO
			// void suballocateImage();

		protected:
			explicit MemoryAllocator(const Device* device);

		// TODO: Don't expose.
		public:
			const Device* device;
			std::vector<MemoryAllocation> allocations;
	};
}