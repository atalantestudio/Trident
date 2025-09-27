#pragma once

namespace ProjectA {
	static GLenum getGlDescriptorType(DescriptorType descriptorType) {
		if (descriptorType == DescriptorType::UNIFORM_BUFFER) {
			return GL_UNIFORM_BUFFER;
		}

		if (descriptorType == DescriptorType::STORAGE_BUFFER) {
			return GL_SHADER_STORAGE_BUFFER;
		}

		ENGINE_ASSERT(false, "OpenGL descriptor type not supported.");

		return 0;
	}
}