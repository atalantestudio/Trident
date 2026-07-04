// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <dxgi1_6.h>

#if defined(DEBUG_DX12_REPORT_LIVE_OBJECTS)
	#include <dxgidebug.h>
#endif

namespace Trident::DX12 {
	struct DescriptorHeapAllocation;

	class Buffer;
	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class ComputePipeline;
	class DescriptorSet;
	class Device;
	class Fence;
	class GraphicsAdapter;
	class GraphicsPipeline;
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class PipelineLayout;
	class Queue;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Swapchain;
}