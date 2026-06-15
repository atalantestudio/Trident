// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"

namespace Trident {
	class GL::MemoryAllocator : public Trident::MemoryAllocator {
		public:
			explicit MemoryAllocator(const Trident::Device* device);
	};

	namespace GL {
		uint32 createMemoryAllocation(Trident::MemoryAllocator* memoryAllocator, uint32 memoryTypeIndex, uint64 size);
	}
}