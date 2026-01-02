// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace ProjectA::DX12 {
	using Microsoft::WRL::ComPtr;

	class CommandBuffer;
	class CommandPool;
	class Device;
	class GraphicsAdapter;
	class Image;
	class Instance;
	class Queue;
	class Surface;
	class Swapchain;
}