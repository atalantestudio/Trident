#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/DescriptorType.hpp"

namespace ProjectA {
	static GLenum getGlDescriptorType(DescriptorType descriptorType) {
		static constexpr GLenum glDescriptorTypes[] = {
			GL_SHADER_STORAGE_BUFFER,
			GL_UNIFORM_BUFFER,
			0,
			0,
		};

		const GLenum glDescriptorType = glDescriptorTypes[static_cast<uint8>(descriptorType)];

		ENGINE_ASSERT(glDescriptorType != 0, "OpenGL descriptor type not supported.");

		return glDescriptorType;
	}
}