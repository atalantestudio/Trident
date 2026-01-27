// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident::GL {
	inline GLenum getBufferDescriptorType(DescriptorType descriptorType) {
		if (descriptorType == DescriptorType::UNIFORM_BUFFER) {
			return GL_UNIFORM_BUFFER;
		}

		if (descriptorType == DescriptorType::STORAGE_BUFFER) {
			return GL_SHADER_STORAGE_BUFFER;
		}

		ADAPTER_ASSERT(false, "This type is not a buffer descriptor type.");

		return 0;
	}
}