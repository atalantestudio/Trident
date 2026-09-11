// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::GL {
	inline GLenum translateIndexFormat(IndexFormat format) {
		static constexpr GLenum INDEX_TYPES[] = {
			GL_UNSIGNED_SHORT,
			GL_UNSIGNED_INT,
		};

		ADAPTER_ASSERT(format < IndexFormat::count, "Unsupported index format.");

		return INDEX_TYPES[static_cast<uint32>(format)];
	}
}