// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Graphics/VertexInputAttributeType.hpp"

namespace Trident::GL {
	// Returns the number of component per generic vertex attribute.
	inline GLint getSize(VertexInputAttributeType type) {
		static constexpr GLint VERTEX_INPUT_ATTRIBUTE_SIZES[] = {
			1,
			1,
			1,
			1,
			1,
			1,
			1,
			2,
			2,
			3,
			3,
			4,
			4,
			0,
			0,
			1,
			2,
			3,
			4,
		};

		const GLint vertexInputAttributeSize = VERTEX_INPUT_ATTRIBUTE_SIZES[static_cast<uint32>(type)];

		ADAPTER_ASSERT(vertexInputAttributeSize != 0, "64-bit unsigned/signed integers as vertex input attributes aren't natively supported.");

		return vertexInputAttributeSize;
	}

	inline GLenum getType(VertexInputAttributeType type) {
		static constexpr GLenum VERTEX_INPUT_ATTRIBUTE_TYPES[] = {
			GL_BYTE,
			GL_UNSIGNED_BYTE,
			GL_SHORT,
			GL_UNSIGNED_SHORT,
			GL_INT,
			GL_UNSIGNED_INT,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_FLOAT,
			GL_UNSIGNED_INT,
			GL_FLOAT,
			0,
			0,
			GL_DOUBLE,
			GL_DOUBLE,
			GL_DOUBLE,
			GL_DOUBLE,
		};

		const GLenum vertexInputAttributeType = VERTEX_INPUT_ATTRIBUTE_TYPES[static_cast<uint32>(type)];

		ADAPTER_ASSERT(vertexInputAttributeType != 0, "64-bit unsigned/signed integers as vertex input attributes aren't natively supported.");

		return vertexInputAttributeType;
	}
}