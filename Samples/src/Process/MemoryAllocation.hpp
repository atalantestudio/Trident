// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Process/Process.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class MemoryAllocationProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				const ref<Device> device = Renderer::getDevice();
				const ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();

				// Create an allocation of 1Kb.
				const uint32 allocationIndex = memoryAllocator->allocate(1000, MemoryProperty::DEVICE_LOCAL);

				// Create a suballocation of 64b.
				const MemorySuballocation suballocation = memoryAllocator->suballocate(allocationIndex, 64, 16);

				const MemoryAllocation& allocation = memoryAllocator->allocations[allocationIndex];

				BREAKPOINT();

				return 0;
			}
	};
}