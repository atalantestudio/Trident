// Copyright 2025 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/PipelineLayout.hpp"
#include "Engine/API/Rendering/ShaderModule.hpp"

namespace Trident {
	struct ComputePipelineDescriptor {
		ref<PipelineLayout> pipelineLayout;
		ref<ShaderModule> computeShaderModule;
	};
}