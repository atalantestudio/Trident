#pragma once

#include "Engine/API/Rendering/DescriptorType.hpp"

namespace ProjectA {
	static GLenum getGlBufferDescriptorType(DescriptorType descriptorType) {
		if (descriptorType == DescriptorType::UNIFORM_BUFFER) {
			return GL_UNIFORM_BUFFER;
		}

		if (descriptorType == DescriptorType::STORAGE_BUFFER) {
			return GL_SHADER_STORAGE_BUFFER;
		}

		ENGINE_ASSERT(false, "This type is not a buffer descriptor type.");

		return 0;
	}
}