// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::GL {
	inline GLenum getIndexType(uint8 indexTypeSize) {
		switch (indexTypeSize) {
			case sizeof(uint16): {
				return GL_UNSIGNED_SHORT;
			}
			case sizeof(uint32): {
				return GL_UNSIGNED_INT;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported index type size.");

				return 0;
			}
		}
	}
}