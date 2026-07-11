// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/Filter.hpp"
#include "Engine/API/Graphics/GraphicsAdapter.hpp"

namespace Trident::GL {
	inline GLenum getFilter(Filter filter) {
		static constexpr GLenum FILTERS[] = {
			GL_NEAREST,
			GL_LINEAR,
		};

		ADAPTER_ASSERT(filter < Filter::count, "Invalid filter.");

		return FILTERS[static_cast<uint32>(filter)];
	}
}