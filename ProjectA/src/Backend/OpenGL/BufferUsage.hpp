#pragma once

namespace ProjectA {
	static GLenum getGlTarget(BufferUsage bufferUsage) {
		if (bufferUsage & BufferUsage::TRANSFER_SRC) {
			return GL_COPY_READ_BUFFER;
		}

		if (bufferUsage & BufferUsage::TRANSFER_DST) {
			return GL_COPY_WRITE_BUFFER;
		}

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

		ENGINE_ASSERT(false, "Unhandled buffer usage.");

		return 0;
	}
}