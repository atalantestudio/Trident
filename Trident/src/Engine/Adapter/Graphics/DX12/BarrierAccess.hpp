// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Access.hpp"
#include "Engine/API/Graphics/ImageLayout.hpp"

namespace Trident::DX12 {
	inline D3D12_BARRIER_ACCESS getBufferBarrierAccess(Access access) {
		if (access == Access::NONE) {
			return D3D12_BARRIER_ACCESS_NO_ACCESS;
		}

		D3D12_BARRIER_ACCESS barrierAccess = D3D12_BARRIER_ACCESS_COMMON;

		if (access & Access::SHADER_READ) {
			barrierAccess |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE | D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
		}

		if (access & Access::SHADER_WRITE) {
			barrierAccess |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE | D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
		}

		if (access & Access::TRANSFER_READ) {
			barrierAccess |= D3D12_BARRIER_ACCESS_COPY_SOURCE;
		}

		if (access & Access::TRANSFER_WRITE) {
			barrierAccess |= D3D12_BARRIER_ACCESS_COPY_DEST;
		}

		return barrierAccess;
	}

	inline D3D12_BARRIER_ACCESS getImageBarrierAccess(Access access, ImageLayout newLayout) {
		if (newLayout == ImageLayout::GENERAL && access & Access::SHADER_WRITE) {
			return D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
		}

		if (newLayout == ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL) {
			return D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ;
		}

		if (access == Access::NONE) {
			return D3D12_BARRIER_ACCESS_NO_ACCESS;
		}

		D3D12_BARRIER_ACCESS barrierAccess = D3D12_BARRIER_ACCESS_COMMON;

		if (access & Access::SHADER_READ) {
			barrierAccess |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		}

		if (access & Access::SHADER_WRITE) {
			if (newLayout == ImageLayout::GENERAL) {
				barrierAccess |= D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
			}
			else {
				barrierAccess |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
			}
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