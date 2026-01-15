// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/ShaderStage.hpp"

namespace ProjectA::DX12 {
	D3D12_SHADER_VISIBILITY getShaderVisibility(ShaderStage shaderStage) {
		if (shaderStage == ShaderStage::VERTEX) {
			return D3D12_SHADER_VISIBILITY_VERTEX;
		}

		if (shaderStage == ShaderStage::FRAGMENT) {
			return D3D12_SHADER_VISIBILITY_PIXEL;
		}

		if (shaderStage == ShaderStage::COMPUTE) {
			return D3D12_SHADER_VISIBILITY_DOMAIN;
		}

		return D3D12_SHADER_VISIBILITY_ALL;
	}
}