#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/IndexType.hpp"

namespace ProjectA {
	static inline GLenum getGlIndexType(const IndexType indexType) {
		static constexpr GLenum glIndexTypes[] = {
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
		};

		return glIndexTypes[static_cast<uint32>(indexType)];
	}

	static inline GLsizei getGlIndexTypeSize(const IndexType indexType) {
		static constexpr GLenum glIndexTypeSizes[] = {
			sizeof(uint16),
			sizeof(uint32),
		};

		return glIndexTypeSizes[static_cast<uint32>(indexType)];
	}
}