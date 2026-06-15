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
				const uint32 allocationIndex = memoryAllocator->createAllocation(0, 1000);

				BREAKPOINT();

				return 0;
			}
	};
}