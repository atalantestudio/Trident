#pragma once

#include "pch.hpp"

#include <glad/glad.h>

namespace ProjectA {
	static inline GLenum getGlBufferBinding(const uint32 bufferUsage) {
		static constexpr GLenum glBufferBindings[] = {
			GL_UNIFORM_BUFFER,
			GL_SHADER_STORAGE_BUFFER,
			GL_ELEMENT_ARRAY_BUFFER,
			GL_ARRAY_BUFFER,
			GL_DRAW_INDIRECT_BUFFER,
		};

		return glBufferBindings[bufferUsage];
	}
}