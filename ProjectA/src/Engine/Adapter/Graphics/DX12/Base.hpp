// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <d3d12.h>
#include <D3Dcompiler.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace ProjectA::DX12 {
	using Microsoft::WRL::ComPtr;

	class CommandBuffer;
	class CommandEncoder;
	class CommandPool;
	class Device;
	class Fence;
	class GraphicsAdapter;
	class Image;
	class Instance;
	class PipelineLayout;
	class Queue;
	class RenderPipeline;
	class ShaderModule;
	class Surface;
	class Swapchain;
}