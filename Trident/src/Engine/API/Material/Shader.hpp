// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/DescriptorSetLayout.hpp"

namespace Trident {
	// TODO(shader-reflection): Replace by ShaderModuleAsset once reflection is ready.
	struct Shader {
		// Internal engine path, relative to "Trident/src/Asset/Shader/".
		sequence<char8> modulePath;

		sequence<DescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	};
}