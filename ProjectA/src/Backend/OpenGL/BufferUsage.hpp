#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	static GLenum getGlBufferTarget(const BufferUsage bufferUsage) {
		static constexpr GLenum glBufferTargets[] = {
			GL_UNIFORM_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
			GL_ELEMENT_ARRAY_BUFFER,
			GL_ARRAY_BUFFER,
			GL_DRAW_INDIRECT_BUFFER,
		};

		return glBufferTargets[static_cast<uint32>(bufferUsage)];
	}
}