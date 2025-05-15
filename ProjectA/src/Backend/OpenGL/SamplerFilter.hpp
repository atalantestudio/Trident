#pragma once

namespace ProjectA {
	static GLenum getGlFilter(SamplerFilter samplerFilter) {
		static constexpr GLenum glFilters[] = {
			GL_NEAREST,
			GL_LINEAR,
		};

		return glFilters[static_cast<uint8>(samplerFilter)];
	}
}