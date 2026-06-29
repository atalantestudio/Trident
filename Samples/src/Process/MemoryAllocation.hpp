// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/MemoryAllocator.hpp"
#include "Engine/API/Graphics/MemoryRequirements.hpp"
#include "Engine/API/Process/Process.hpp"
#include "Engine/Renderer/Renderer.hpp"

namespace Trident {
	class MemoryAllocationProcess final : public Process {
		public:
			sint32 main(view<void*> arguments) override {
				const ref<Device> device = Renderer::getDevice();
				const ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();

				const uint32 memoryTypeIndex = memoryAllocator->findMemoryTypeIndex(MemoryProperty::DEVICE_LOCAL, false);

				const uint32 allocationIndex = memoryAllocator->allocate(false, memoryTypeIndex, 1000, 1);
				const MemoryAllocation& allocation = memoryAllocator->allocations[allocationIndex];

				const ref<BaseBuffer> buffer1 = device->createStaticBuffer({
					.size = 256,
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Buffer 1",
					#endif
				});

				const MemoryRequirements buffer1MemoryRequirements = device->adapter->getBufferMemoryRequirementsImplementation(memoryAllocator.get(), buffer1);

				memoryAllocator->suballocateStatic(allocationIndex, buffer1);

				const ref<BaseBuffer> buffer2 = device->createStaticBuffer({
					.size = 87,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Buffer 2",
					#endif
				});

				const MemoryRequirements buffer2MemoryRequirements = device->adapter->getBufferMemoryRequirementsImplementation(memoryAllocator.get(), buffer2);

				memoryAllocator->suballocateStatic(allocationIndex, buffer2);

				BREAKPOINT();

				return 0;
			}
	};
}