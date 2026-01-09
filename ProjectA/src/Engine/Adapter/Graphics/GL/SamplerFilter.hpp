// Copyright 2026 Atalante Studio.
// Distributed under the GNU GPL v3 License.

#pragma once

#include "Engine/API/Rendering/SamplerFilter.hpp"

namespace ProjectA::GL {
	inline GLenum getFilter(SamplerFilter samplerFilter) {
		static constexpr GLenum glFilters[] = {
			GL_NEAREST,
			GL_LINEAR,
		};

		return glFilters[static_cast<uint8>(samplerFilter)];
	}
}