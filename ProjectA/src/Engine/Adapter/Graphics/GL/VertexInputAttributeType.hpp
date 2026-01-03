#pragma once

#include "Engine/API/Rendering/VertexInputAttributeType.hpp"

namespace ProjectA {
	// Returns the number of component per generic vertex attribute.
	GLint getGlSize(VertexInputAttributeType type) {
		static constexpr GLint VERTEX_INPUT_ATTRIBUTE_SIZES[] = {
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			2,
			3,
			4,
			0,
			0,
			1,
			2,
			3,
			4,
		};

		const GLint glSize = VERTEX_INPUT_ATTRIBUTE_SIZES[static_cast<uint32>(type)];

		if (glSize == 0) {
			ENGINE_ERROR("64-bit unsigned/signed integers as vertex input attributes aren't supported by OpenGL.");
		}

		return glSize;
	}

	GLenum getGlType(VertexInputAttributeType type) {
		static constexpr GLenum VERTEX_INPUT_ATTRIBUTE_TYPES[] = {
			GL_BYTE,
			GL_UNSIGNED_BYTE,
			GL_SHORT,
			GL_UNSIGNED_SHORT,
			GL_INT,
			GL_UNSIGNED_INT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			GL_FLOAT,
			0,
			0,
			GL_DOUBLE,
			GL_DOUBLE,
			GL_DOUBLE,
			GL_DOUBLE,
		};

		const GLenum glType = VERTEX_INPUT_ATTRIBUTE_TYPES[static_cast<uint32>(type)];

		if (glType == 0) {
			ENGINE_ERROR("64-bit unsigned/signed integers as vertex input attributes aren't supported by OpenGL.");
		}

		return glType;
	}
}