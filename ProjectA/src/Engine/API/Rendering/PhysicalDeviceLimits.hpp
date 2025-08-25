#pragma once

namespace ProjectA {
	struct PhysicalDeviceLimits {
		// Granularity in bytes at which buffer/linear image resources and optimal image resources
		// can be bound to adjacent offsets in the same device memory without aliasing.
		// Only relevant when using the Vulkan adapter.
		uint64 bufferImageGranularity;

		// Granularity of supported line widths.
		float32 lineWidthGranularity;

		// Maximum number of color attachments that can be used by a subpass in a render pass.
		uint32 maxColorAttachmentCount;

		// Maximum number of descriptor sets that can be simultaneously used by a pipeline.
		// Only relevant when using the Vulkan adapter.
		uint32 maxBoundDescriptorSetCount;

		// Maximum number of local workgroups that can be dispatched by a single dispatching command.
		vec3<uint32> maxComputeWorkgroupCount;

		// Maximum size of a local compute workgroup, per dimension.
		vec3<uint32> maxComputeWorkgroupSize;

		// Maximum size for a framebuffer.
		vec2<uint32> maxFramebufferSize;

		// Maximum layer count for a layered framebuffer.
		uint32 maxFramebufferLayerCount;

		// The largest dimension that is required to be supported for 1D images.
		uint32 maxImageDimension1D;

		// The largest dimension that (width or height) is required to be supported for 2D images.
		uint32 maxImageDimension2D;

		// The largest dimension that (width, height or depth) is required to be supported for 3D images.
		uint32 maxImageDimension3D;

		// Maximum number of layers for an image.
		uint32 maxImageLayerCount;

		// Maximum number of device memory allocations which can simultaneously exist.
		// Only relevant when using the Vulkan adapter.
		uint32 maxMemoryAllocationCount;

		// Maximum number of vertex input attributes that can be specified for a graphics pipeline.
		uint32 maxVertexInputAttributeCount;

		// Maximum vertex input attribute offset that can be added to the vertex input binding stride.
		uint32 maxVertexInputAttributeOffset;

		// Maximum number of vertex buffers that can be specified for providing vertex input attributes to a graphics pipeline.
		uint32 maxVertexInputBindingCount;

		// Maximum vertex input binding stride that can be specified in a vertex input binding.
		uint32 maxVertexInputBindingStride;

		// Granularity of supported point sizes.
		float32 pointSizeGranularity;
	};
}