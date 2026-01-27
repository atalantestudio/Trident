// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"
#include "Engine/API/Rendering/BufferUsage.hpp"

namespace Trident::GL {
	inline GLenum getTarget(BufferUsage bufferUsage) {
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

		if (bufferUsage & BufferUsage::INDIRECT) {
			return GL_DRAW_INDIRECT_BUFFER;
		}

		ADAPTER_ASSERT(false, "Unhandled buffer usage.");

		return 0;
	}
}