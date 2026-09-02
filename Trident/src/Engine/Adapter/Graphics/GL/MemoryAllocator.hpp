// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Device.hpp"
#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/MemoryRequirements.hpp"

namespace Trident {
	class GL::MemoryAllocator : public Trident::MemoryAllocator {
		public:
			explicit MemoryAllocator(const Trident::Device* device);
	};

	namespace GL {
		MemoryAllocation allocateMemory(Trident::MemoryAllocator* memoryAllocator, bool dynamic, uint32 memoryTypeIndex, uint64 size, uint64 alignment, bool shaderDeviceAddress);

		uint32 findMemoryTypeIndex(const Trident::MemoryAllocator* memoryAllocator, MemoryProperty memoryProperties, bool transferDestination);

		MemoryRequirements getBufferMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Buffer> buffer);

		MemoryRequirements getImageMemoryRequirements(const Trident::MemoryAllocator* memoryAllocator, ref<Trident::Image> image);

		MemorySuballocation suballocateBufferMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Buffer> buffer);

		MemorySuballocation suballocateImageMemory(Trident::MemoryAllocator* memoryAllocator, uint32 allocationIndex, ref<Trident::Image> image);
	}
}