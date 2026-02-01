// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/DescriptorType.hpp"

namespace Trident::GL {
	inline GLenum getBufferDescriptorType(DescriptorType descriptorType) {
		switch (descriptorType) {
			case DescriptorType::UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
			case DescriptorType::READ_ONLY_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;
			case DescriptorType::READ_WRITE_STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;
			default: {
				ADAPTER_ASSERT(false, "The provided type is either unknown or not a buffer descriptor type.");

				return 0;
			}
		}
	}
}