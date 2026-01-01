#pragma once

namespace ProjectA {
	static GLenum getGlIndexType(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16):
				return GL_UNSIGNED_SHORT;
			case sizeof(uint32):
				return GL_UNSIGNED_INT;
			default:
				ENGINE_ASSERT(false, "Unsupported index type size.");

				return 0;
		}
	}
}