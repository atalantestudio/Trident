// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"
#include "Engine/API/Rendering/ImageLayout.hpp"
#include "Engine/API/Rendering/ImageUsage.hpp"

namespace ProjectA::DX12 {
	inline D3D12_RESOURCE_STATES getBufferResourceStates(BufferUsage bufferUsage) {
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;

		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			resourceStates |= D3D12_RESOURCE_STATE_COPY_SOURCE;
		}

		// TODO: D3D12_RESOURCE_STATE_COPY_DEST can only be used with D3D12_HEAP_TYPE_READBACK.
		/* if (bufferUsage & BufferUsage::TRANSFER_DST) {
			resourceStates |= D3D12_RESOURCE_STATE_COPY_DEST;
		} */

		if (bufferUsage & BufferUsage::UNIFORM) {
			resourceStates |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (bufferUsage & BufferUsage::STORAGE) {
			resourceStates |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (bufferUsage & BufferUsage::INDEX) {
			resourceStates |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
		}

		// TODO: It seems this type cannot be used as the initial resource state and must instead be transitioned into via a resource barrier.
		/* if (bufferUsage & BufferUsage::VERTEX) {
			resourceStates |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		} */

		if (bufferUsage & BufferUsage::INDIRECT) {
			resourceStates |= D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT;
		}

		return resourceStates;
	}

	inline D3D12_RESOURCE_STATES getImageResourceStates(ImageUsage imageUsage, ImageLayout& layout) {
		D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;

		if (imageUsage & ImageUsage::TRANSFER_SRC) {
			resourceStates |= D3D12_RESOURCE_STATE_COPY_SOURCE;

			layout = ImageLayout::TRANSFER_SRC_OPTIMAL;
		}

		if (imageUsage & ImageUsage::TRANSFER_DST) {
			resourceStates |= D3D12_RESOURCE_STATE_COPY_DEST;

			layout = ImageLayout::TRANSFER_DST_OPTIMAL;
		}

		if (imageUsage & ImageUsage::STORAGE) {
			resourceStates |= D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
		}

		if (imageUsage & ImageUsage::COLOR_ATTACHMENT) {
			resourceStates |= D3D12_RESOURCE_STATE_RENDER_TARGET;

			layout = ImageLayout::COLOR_ATTACHMENT_OPTIMAL;
		}

		if (imageUsage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			resourceStates |= D3D12_RESOURCE_STATE_RENDER_TARGET;

			layout = ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		}

		if (imageUsage & ImageUsage::INPUT_ATTACHMENT) {
			resourceStates |= D3D12_RESOURCE_STATE_RENDER_TARGET;
		}

		return resourceStates;
	}

	inline D3D12_RESOURCE_STATES getImageTransitionResourceStates(ImageLayout imageLayout) {
		switch (imageLayout) {
			case ImageLayout::UNDEFINED: return D3D12_RESOURCE_STATE_COMMON;
			case ImageLayout::GENERAL: return D3D12_RESOURCE_STATE_GENERIC_READ;
			case ImageLayout::COLOR_ATTACHMENT_OPTIMAL: return D3D12_RESOURCE_STATE_RENDER_TARGET;
			case ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return D3D12_RESOURCE_STATE_RENDER_TARGET | D3D12_RESOURCE_STATE_DEPTH_WRITE;
			case ImageLayout::DEPTH_STENCIL_READ_ONLY_OPTIMAL: return D3D12_RESOURCE_STATE_DEPTH_READ;
			case ImageLayout::SHADER_READ_ONLY_OPTIMAL: {
				// TODO: Include D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE?
				return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
			}
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