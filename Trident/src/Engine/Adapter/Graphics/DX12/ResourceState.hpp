// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_STATES getBufferResourceState(BufferUsage bufferUsage) {
		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			return D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		if (bufferUsage & BufferUsage::UNIFORM) {
			return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		return D3D12_RESOURCE_STATE_COMMON;
	}

	inline D3D12_RESOURCE_STATES getImageResourceState(ImageUsage usage, ImageLayout& layout, uint8 sampleCount) {
		// TODO: Restore.
		return D3D12_RESOURCE_STATE_COMMON;

		/* if (sampleCount > 1) {
			if (usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			}

			return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		}

		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;

		if (usage & ImageUsage::TRANSFER_SRC) {
			resourceStates |= D3D12_RESOURCE_STATE_COPY_SOURCE;

			layout = ImageLayout::TRANSFER_SRC_OPTIMAL;
		}

		if (usage & ImageUsage::STORAGE) {
			resourceStates |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

			layout = ImageLayout::GENERAL;
		}

		if (usage & ImageUsage::COLOR_ATTACHMENT) {
			resourceStates |= D3D12_RESOURCE_STATE_RENDER_TARGET;

			layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
		}

		if (usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			resourceStates |= D3D12_RESOURCE_STATE_DEPTH_WRITE;

			layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		return resourceStates; */
	}

	inline D3D12_RESOURCE_STATES getImageTransitionResourceState(ImageLayout imageLayout) {
		static constexpr D3D12_RESOURCE_STATES RESOURCE_STATES[] = {
			D3D12_RESOURCE_STATE_COMMON,

			// TODO: Should resources in GENERAL layout have D3D12_RESOURCE_STATE_UNORDERED_ACCESS instead?
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,

			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_RENDER_TARGET | D3D12_RESOURCE_STATE_DEPTH_WRITE,
			D3D12_RESOURCE_STATE_DEPTH_READ,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_SOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_PRESENT,
		};

		ADAPTER_ASSERT(imageLayout < ImageLayout::count, "Invalid image layout.");

		return RESOURCE_STATES[static_cast<uint32>(imageLayout)];
	}
}