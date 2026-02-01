// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace Trident {
	struct MaterialPipelineDescriptor {
		// Path to an internal shader module. Relative to Trident/src/Asset/Shader/.
		view<char8> shaderModulePath;

		view<DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	};
}