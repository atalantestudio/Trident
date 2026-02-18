// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Access.hpp"

namespace Trident::DX12 {
	inline D3D12_BARRIER_ACCESS getBarrierAccess(Access access) {
		D3D12_BARRIER_ACCESS barrierAccess{};

		if (access & Access::SHADER_READ) {
			barrierAccess |= D3D12_BARRIER_ACCESS_UNORDERED_ACCESS | D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		}

		if (access & Access::SHADER_WRITE) {
			barrierAccess |= D3D12_BARRIER_ACCESS_UNORDERED_ACCESS | D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		}

		if (access & Access::TRANSFER_READ) {
			barrierAccess |= D3D12_BARRIER_ACCESS_COPY_SOURCE;
		}

		if (access & Access::TRANSFER_WRITE) {
			barrierAccess |= D3D12_BARRIER_ACCESS_COPY_DEST;
		}

		return barrierAccess;
	}
}