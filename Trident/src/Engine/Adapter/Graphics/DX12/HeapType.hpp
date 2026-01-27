// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/MemoryProperty.hpp"

namespace Trident::DX12 {
	inline D3D12_HEAP_TYPE getHeapType(MemoryProperty memoryProperties) {
		if (memoryProperties & MemoryProperty::HOST_VISIBLE) {
			return D3D12_HEAP_TYPE_UPLOAD;
		}

		if (memoryProperties & MemoryProperty::HOST_COHERENT) {
			return D3D12_HEAP_TYPE_UPLOAD;
		}

		ADAPTER_ASSERT(memoryProperties & MemoryProperty::DEVICE_LOCAL, "Memory properties must contain DEVICE_LOCAL.");

		return D3D12_HEAP_TYPE_DEFAULT;
	}
}