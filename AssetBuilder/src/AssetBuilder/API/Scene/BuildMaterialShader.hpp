// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/DescriptorSetLayout.hpp"
#include "Engine/API/Graphics/ShaderStage.hpp"

namespace Trident {
	struct BuildMaterialShader {
		sequence<char8> name;
		ShaderStage stage;
		sequence<char8> entryPoint;
		sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
		range<uint32, uint32> materialAssetRange;
	};
}