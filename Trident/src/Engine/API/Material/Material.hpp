// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Shader.hpp"

namespace Trident {
	struct MaterialDescriptorImage {
		sequence<char8> assetPath;
	};

	struct MaterialWriteDescriptorSetBinding {
		sequence<MaterialDescriptorImage> descriptorImages;
	};

	struct Material {
		uint32 shaderIndex;
		sequence<MaterialWriteDescriptorSetBinding> writeDescriptorSetBindings;
	};
}