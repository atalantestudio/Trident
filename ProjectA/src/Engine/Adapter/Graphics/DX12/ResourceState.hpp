// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/BufferUsage.hpp"

namespace ProjectA::DX12 {
	inline D3D12_RESOURCE_STATES getResourceState(BufferUsage bufferUsage) {
		D3D12_RESOURCE_STATES resourceState{};

		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			resourceState |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		if (bufferUsage & BufferUsage::TRANSFER_DST) {
			resourceState |= D3D12_RESOURCE_STATE_COPY_DEST;
		}

		if (bufferUsage & BufferUsage::UNIFORM) {
			resourceState |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (bufferUsage & BufferUsage::STORAGE) {
			resourceState |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (bufferUsage & BufferUsage::INDEX) {
			resourceState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		}

		if (bufferUsage & BufferUsage::VERTEX) {
			resourceState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		}

		if (bufferUsage & BufferUsage::INDIRECT) {
			resourceState |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		}

		return resourceState;
	}
}