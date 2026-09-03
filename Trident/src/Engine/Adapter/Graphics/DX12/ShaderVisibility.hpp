// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident::DX12 {
	inline D3D12_SHADER_VISIBILITY getShaderVisibility(ShaderStage stage) {
		if (stage == ShaderStage::VERTEX) {
			return D3D12_SHADER_VISIBILITY_VERTEX;
		}

		if (stage == ShaderStage::FRAGMENT) {
			return D3D12_SHADER_VISIBILITY_PIXEL;
		}

		if (stage == ShaderStage::COMPUTE) {
			// Compute queues always use D3D12_SHADER_VISIBILITY_ALL because they have only one active shader stage.
			// See https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_shader_visibility#remarks.
			return D3D12_SHADER_VISIBILITY_ALL;
		}

		return D3D12_SHADER_VISIBILITY_ALL;
	}
}