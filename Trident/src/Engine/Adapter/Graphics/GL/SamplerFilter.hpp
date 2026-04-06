// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Graphics/SamplerFilter.hpp"

namespace Trident::GL {
	inline GLenum getFilter(SamplerFilter samplerFilter) {
		static constexpr GLenum FILTERS[] = {
			GL_NEAREST,
			GL_LINEAR,
		};

		return FILTERS[static_cast<uint32>(samplerFilter)];
	}
}