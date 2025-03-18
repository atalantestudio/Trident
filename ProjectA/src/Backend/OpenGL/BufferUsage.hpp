#pragma once

#include <glad/glad.h>

#include "Engine/Renderer/Buffer/BufferUsage.hpp"

namespace ProjectA {
	static GLenum getGlBufferTarget(BufferUsage bufferUsage) {
		// TODO: Find a better way of converting an enum to another.
		if (bufferUsage & BufferUsage::UNIFORM) {
			return GL_UNIFORM_BUFFER;
		}

		if (bufferUsage & BufferUsage::STORAGE) {
			return GL_SHADER_STORAGE_BUFFER;
		}

		if (bufferUsage & BufferUsage::INDEX) {
			return GL_ELEMENT_ARRAY_BUFFER;
		}

		if (bufferUsage & BufferUsage::VERTEX) {
			return GL_ARRAY_BUFFER;
		}

		if (bufferUsage & BufferUsage::INDIRECT) {
			return GL_DRAW_INDIRECT_BUFFER;
		}

		// TRANSFER_SRC and TRANSFER_DST are secondary usages;
		// only return these in last resort.

		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			return GL_COPY_READ_BUFFER;
		}

		if (bufferUsage & BufferUsage::TRANSFER_DST) {
			return GL_COPY_WRITE_BUFFER;
		}

		throw std::runtime_error("Failed to convert buffer usage.");
	}
}