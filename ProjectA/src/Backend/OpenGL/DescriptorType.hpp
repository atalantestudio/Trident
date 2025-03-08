#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	static GLenum getGlDescriptorType(const DescriptorType descriptorType) {
		static constexpr GLenum glDescriptorTypes[] = {
			GL_SHADER_STORAGE_BUFFER,
			GL_UNIFORM_BUFFER,
			0,
		};

		const GLenum glDescriptorType = glDescriptorTypes[static_cast<uint32>(descriptorType)];

		if (glDescriptorType == 0) {
			throw std::runtime_error("OpenGL descriptor type not supported.");
		}

		return glDescriptorType;
	}
}