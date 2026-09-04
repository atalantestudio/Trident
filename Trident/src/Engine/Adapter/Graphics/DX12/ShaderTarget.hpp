// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident::DX12 {
	inline LPCSTR getShaderTarget(ShaderStage stage) {
		switch (stage) {
			case ShaderStage::VERTEX: {
				return "vs_5_0";
			}
			case ShaderStage::FRAGMENT: {
				return "ps_5_0";
			}
			case ShaderStage::COMPUTE: {
				return "cs_5_0";
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported shader stage.");

				return nullptr;
			}
		}
	}
}