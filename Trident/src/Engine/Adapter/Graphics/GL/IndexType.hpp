// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::GL {
	inline GLenum translateIndexFormat(IndexFormat format) {
		switch (format) {
			case R16_UINT: {
				return GL_UNSIGNED_SHORT;
			}
			case R32_UINT: {
				return GL_UNSIGNED_INT;
			}
			default: {
				ADAPTER_ASSERT(false, "Unsupported index format.");

				return 0;
			}
		}
	}
}