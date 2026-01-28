// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_STATES getBufferResourceStates(BufferUsage bufferUsage) {
		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			return D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		if (bufferUsage & BufferUsage::UNIFORM) {
			return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		return D3D12_RESOURCE_STATE_COMMON;
	}

	inline D3D12_RESOURCE_STATES getImageResourceStates(ImageUsage usage, ImageLayout& layout, uint8 sampleCount) {
		if (sampleCount > 1) {
			if (usage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
			}

			return D3D12_RESOURCE_STATE_RESOLVE_SOURCE;
		}

		if (usage & ImageUsage::INPUT_ATTACHMENT) {
			return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
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

		return resourceStates;
	}

	inline D3D12_RESOURCE_STATES getImageTransitionResourceStates(ImageLayout imageLayout) {
		switch (imageLayout) {
			case ImageLayout::UNDEFINED: return D3D12_RESOURCE_STATE_COMMON;
			case ImageLayout::GENERAL: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
			case ImageLayout::COLOR_ATTACHMENT_OPTIMAL: return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return D3D12_RESOURCE_STATE_RENDER_TARGET | D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
			case ImageLayout::TRANSFER_SRC_OPTIMAL: return D3D12_RESOURCE_STATE_COPY_SOURCE;
			case ImageLayout::TRANSFER_DST_OPTIMAL: return D3D12_RESOURCE_STATE_COPY_DEST;
			case ImageLayout::PRESENT_SRC: return D3D12_RESOURCE_STATE_PRESENT;
			default: {
				ADAPTER_ASSERT(false, "Unknown image layout.");

				return D3D12_RESOURCE_STATE_COMMON;
			}
		}
	}
}