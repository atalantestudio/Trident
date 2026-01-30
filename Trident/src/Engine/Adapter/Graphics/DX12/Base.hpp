// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

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
	class Framebuffer;
	class GraphicsAdapter;
	class Image;
	class ImageView;
	class Instance;
	class MemoryAllocator;
	class PhysicalDevice;
	class PipelineLayout;
	class Queue;
	class RenderPipeline;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Swapchain;
}