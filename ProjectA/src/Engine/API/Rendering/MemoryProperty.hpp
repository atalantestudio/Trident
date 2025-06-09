#pragma once

namespace ProjectA {
	// See https://zeux.io/2020/02/27/writing-an-efficient-vulkan-renderer.
	// Memory properties. Useful combinations:
	// - DEVICE_LOCAL: GPU memory that is fast to access from the GPU but not visible from the CPU. Suited for static resources and attachments.
	// - DEVICE_LOCAL | HOST_VISIBLE: (AMD only) Up to 256Mb of GPU memory that can be written to by the CPU. Useful for data that changes every frame.
	// - HOST_VISIBLE | HOST_COHERENT: CPU memory that is visible from the GPU. It is suited for data that changes every frame.
	// - DEVICE_LOCAL | LAZILY_ALLOCATED: GPU memory that can be lazily allocated. Use for large attachments that are not stored to.
	// - HOST_CACHED: CPU memory. Useful for reading back data written to by GPU.
	enum class MemoryProperty : uint8 {
		// Specifies that memory allocated with this type is the most efficient for device access.
		DEVICE_LOCAL = BIT(0),

		// Specifies that memory allocated with this type can be mapped for host access.
		HOST_VISIBLE = BIT(1),

		// Specifies that the host cache management commands are not needed to manage availability and visibility on the host.
		HOST_COHERENT = BIT(2),

		// Specifies that memory allocated with this type is cached on the host.
		// Host memory accesses to uncached memory are slower than to cached memory, however uncached memory is always host coherent.
		HOST_CACHED = BIT(3),

		// Specifies that the memory type only allows device access to the memory.
		LAZILY_ALLOCATED = BIT(4),
	};

	AS_FLAG(MemoryProperty);
}