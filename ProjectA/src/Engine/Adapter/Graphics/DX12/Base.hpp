// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <d3d12.h>
#include <d3dcommon.h>
#include <D3Dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace ProjectA::DX12 {
	using Microsoft::WRL::ComPtr;

	class Buffer;
	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class Device;
	class Fence;
	class Framebuffer;
	class GraphicsAdapter;
	class Image;
	class ImageView;
	class Instance;
	class PhysicalDevice;
	class PipelineLayout;
	class Queue;
	class RenderPipeline;
	class Sampler;
	class ShaderModule;
	class Surface;
	class Swapchain;
}