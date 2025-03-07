#pragma once

#include <glad/glad.h>

namespace ProjectA {
	static GLenum getGlBufferTarget(const uint32 bufferUsage) {
		static constexpr GLenum glBufferTargets[] = {
			GL_UNIFORM_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
			GL_ELEMENT_ARRAY_BUFFER,
			GL_ARRAY_BUFFER,
			GL_DRAW_INDIRECT_BUFFER,
		};

		return glBufferTargets[bufferUsage];
	}
}