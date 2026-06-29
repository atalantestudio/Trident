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
				constexpr MemoryProperty memoryProperties = MemoryProperty::HOST_VISIBLE | MemoryProperty::HOST_COHERENT;

				const ref<Device> device = Renderer::getDevice();
				const ref<MemoryAllocator> memoryAllocator = device->getMemoryAllocator();

				const ref<BaseBuffer> buffer1 = device->createStaticBuffer({
					.size = 256,
					.usage = BufferUsage::UNIFORM,
					.memoryProperties = memoryProperties,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Buffer 1",
					#endif
				});
				const ref<BaseBuffer> buffer2 = device->createStaticBuffer({
					.size = 87,
					.usage = BufferUsage::TRANSFER_SRC,
					.memoryProperties = memoryProperties,
					.persistent = false,
					#if defined(DEBUG_OBJECT_NAME)
						.name = "Buffer 2",
					#endif
				});

				const MemoryRequirements buffer1memoryRequirements = memoryAllocator->getMemoryRequirements(buffer1);
				const MemoryRequirements buffer2memoryRequirements = memoryAllocator->getMemoryRequirements(buffer2);

				const uint32 allocationMemoryTypeIndex = memoryAllocator->findMemoryTypeIndex(memoryProperties, false);
				const uint64 allocationAlignment = max(buffer1memoryRequirements.alignment, buffer2memoryRequirements.alignment);
				const uint64 allocationSize = ceil(buffer1memoryRequirements.size + buffer2memoryRequirements.size, allocationAlignment);
				const uint32 allocationIndex = memoryAllocator->allocate(false, allocationMemoryTypeIndex, allocationSize, allocationAlignment);
				const MemoryAllocation& allocation = memoryAllocator->allocations[allocationIndex];

				memoryAllocator->suballocateStatic(allocationIndex, buffer1);
				memoryAllocator->suballocateStatic(allocationIndex, buffer2);

				return 0;
			}
	};
}