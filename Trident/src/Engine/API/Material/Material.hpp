// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace Trident {
	struct Material {
		// Path to an internal shader module. Relative to "Trident/src/Asset/Shader/".
		sequence<char8> shaderModulePath;

		sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	};
}