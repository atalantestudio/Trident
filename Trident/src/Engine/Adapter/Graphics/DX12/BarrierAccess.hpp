// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/Access.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"

namespace Trident::DX12 {
	inline D3D12_BARRIER_ACCESS getBufferBarrierAccess(Access access) {
		if (access == Access::NONE) {
			return D3D12_BARRIER_ACCESS_NO_ACCESS;
		}

		D3D12_BARRIER_ACCESS barrierAccess = D3D12_BARRIER_ACCESS_COMMON;

		if (access & Access::SHADER_READ) {
			// TODO
			barrierAccess |= /* D3D12_BARRIER_ACCESS_UNORDERED_ACCESS */ D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		}

		if (access & Access::SHADER_WRITE) {
			// TODO
			barrierAccess |= /* D3D12_BARRIER_ACCESS_UNORDERED_ACCESS */ D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
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
		D3D12_BARRIER_ACCESS barrierAccess = D3D12_BARRIER_ACCESS_COMMON;

		if (access & Access::SHADER_READ) {
			if (newLayout == ImageLayout::GENERAL) {
				barrierAccess |= D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
			}
			else {
				barrierAccess |= D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
			}
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