#pragma once

namespace ProjectA {
	// Returns the number of component per generic vertex attribute.
	static GLint getGlSize(VertexInputAttributeType type) {
		static constexpr GLint glSizes[] = {
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

		const GLint glSize = glSizes[static_cast<uint32>(type)];

		if (glSize == 0) {
			ENGINE_ERROR("64-bit unsigned/signed integers as vertex input attributes aren't supported by OpenGL.");
		}

		return glSize;
	}

	static GLenum getGlType(VertexInputAttributeType type) {
		static constexpr GLenum glTypes[] = {
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

		const GLenum glType = glTypes[static_cast<uint32>(type)];

		if (glType == 0) {
			ENGINE_ERROR("64-bit unsigned/signed integers as vertex input attributes aren't supported by OpenGL.");
		}

		return glType;
	}
}