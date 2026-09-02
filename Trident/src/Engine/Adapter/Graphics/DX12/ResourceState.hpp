// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/BufferUsage.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_STATES translateBufferUsage(BufferUsage usage, MemoryProperty memoryProperties) {
		if (memoryProperties & MemoryProperty::HOST_VISIBLE) {
			return D3D12_RESOURCE_STATE_GENERIC_READ;

			if (usage == BufferUsage::TRANSFER_DST) {
				return D3D12_RESOURCE_STATE_COPY_DEST;
			}
		}

		if (usage & BufferUsage::ACCELERATION_STRUCTURE_STORAGE) {
			return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		}

		return D3D12_RESOURCE_STATE_COMMON;
	}
}