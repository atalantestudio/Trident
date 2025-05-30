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
		DEVICE_LOCAL = BIT(0),
		HOST_VISIBLE = BIT(1),
		HOST_COHERENT = BIT(2),
		HOST_CACHED = BIT(3),
		LAZILY_ALLOCATED = BIT(4),
	};

	AS_FLAG(MemoryProperty);
}