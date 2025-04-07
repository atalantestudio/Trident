#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/IndexType.hpp"

namespace ProjectA {
	static GLenum getGlIndexType(IndexType indexType) {
		static constexpr GLenum glIndexTypes[] = {
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
		};

		return glIndexTypes[static_cast<uint8>(indexType)];
	}

	static GLsizei getGlIndexTypeSize(IndexType indexType) {
		static constexpr GLenum glIndexTypeSizes[] = {
			sizeof(uint16),
			sizeof(uint32),
		};

		return glIndexTypeSizes[static_cast<uint8>(indexType)];
	}
}