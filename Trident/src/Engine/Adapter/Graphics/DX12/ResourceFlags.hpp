// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ImageUsage.hpp"

namespace Trident::DX12 {
	inline D3D12_RESOURCE_FLAGS getImageResourceFlags(ImageUsage imageUsage) {
		D3D12_RESOURCE_FLAGS resourceFlags = D3D12_RESOURCE_FLAG_NONE;

		if (imageUsage & ImageUsage::STORAGE) {
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		if (imageUsage & ImageUsage::COLOR_ATTACHMENT) {
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if (imageUsage & ImageUsage::DEPTH_STENCIL_ATTACHMENT) {
			resourceFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		}

		return resourceFlags;
	}
}