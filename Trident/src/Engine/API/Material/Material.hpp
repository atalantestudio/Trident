// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Material/Shader.hpp"

namespace Trident {
	struct MaterialDescriptorBuffer {
		sequence<uint8> data;
	};

	struct MaterialDescriptorImage {
		sequence<char8> assetPath;
	};

	struct MaterialWriteDescriptorSetBinding {
		// TODO: Use an union.
		sequence<MaterialDescriptorBuffer> descriptorBuffers;
		sequence<MaterialDescriptorImage> descriptorImages;
	};

	struct Material {
		uint32 shaderIndex;
		sequence<MaterialWriteDescriptorSetBinding> writeDescriptorSetBindings;
	};
}